/*
 * Copyright (C) 2026 MarcusPy827
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "src/info_server/network_control/network_control.h"

#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusReply>
#include <QUuid>
#include <QProcess>

#include <algorithm>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

namespace {

// Variant-map type used by NM's AddAndActivateConnection method.
// a{sa{sv}} = dict of dicts
using NmSettingsMap = QMap<QString, QVariantMap>;

}  // namespace

// Construction / initialization

NetworkControl::NetworkControl(QObject* parent)
    : QObject(parent), bus_(QDBusConnection::systemBus()) {
  Initialize();
}

void NetworkControl::Initialize() {
  // Subscribe to NM-level property changes (WirelessEnabled, Connectivity...)
  bus_.connect(
    DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_PROPERTIES_INTERFACE,
    DBUS_PROP_UPDTE_METHOD,
    this,
    SLOT(OnNmPropertiesChanged(QString, QVariantMap, QStringList)));

  // Subscribe to NM state changes
  bus_.connect(
    DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_NETWORK_MANAGER_INTERFACE,
    "StateChanged",
    this,
    SLOT(OnNmStateChanged(uint)));

  // Subscribe to device hotplug events
  bus_.connect(
    DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_NETWORK_MANAGER_INTERFACE,
    "DeviceAdded",
    this,
    SLOT(OnDeviceAdded(QDBusObjectPath)));

  bus_.connect(
    DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_NETWORK_MANAGER_INTERFACE,
    "DeviceRemoved",
    this,
    SLOT(OnDeviceRemoved(QDBusObjectPath)));

  RefreshDevices();
}

// ---------------------------------------------------------------------------
// Getters
// ---------------------------------------------------------------------------

bool NetworkControl::HasWlanHardware() const { return has_wlan_; }
bool NetworkControl::HasEthernetHardware() const { return has_ethernet_; }
bool NetworkControl::IsLoginRequired() const { return login_required_; }
bool NetworkControl::IsAirplaneModeEnabled() const { return airplane_mode_; }

QString NetworkControl::GetEthernetIcon() const {
  for (const QVariant& entry : ethernet_devices_) {
    const QVariantMap dev = entry.toMap();
    if (dev.value("isConnected").toBool())
      return QStringLiteral("settings_ethernet");
  }
  return QStringLiteral("globe_2_cancel");
}

QString NetworkControl::GetWlanIcon() const {
  if (!current_wlan_network_.isEmpty()) {
    if (current_wlan_network_.value("status").toString()
        == QLatin1String("no_internet"))
      return QStringLiteral("wifi_find");
    return current_wlan_network_.value("icon").toString();
  }
  return QStringLiteral("signal_wifi_off");
}

QVariantList NetworkControl::GetWlanDevices() const { return wlan_devices_; }

QString NetworkControl::GetSelectedWlanDevice() const {
  return selected_wlan_device_path_;
}

bool NetworkControl::IsSelectedWlanDeviceEnabled() const {
  QVariant state = GetNmProperty(
    selected_wlan_device_path_,
    DBUS_NETWORK_MANAGER_DEVICE_INTERFACE,
    "State");
  uint s = state.toUInt();
  return s != NC_DEVICE_STATE_UNAVAILABLE
      && s != NC_DEVICE_STATE_UNMANAGED
      && s != NC_DEVICE_STATE_UNKNOWN;
}

QVariantMap NetworkControl::GetCurrentWlanNetwork() const {
  return current_wlan_network_;
}

QVariantList NetworkControl::GetAvailableWlanNetworks() const {
  return available_wlan_networks_;
}

QVariantList NetworkControl::GetEthernetDevices() const {
  return ethernet_devices_;
}

bool NetworkControl::IsHotspotEnabled() const { return hotspot_enabled_; }
bool NetworkControl::IsHotspotAvailable() const { return hotspot_available_; }
QString NetworkControl::GetHotspotName() const { return hotspot_name_; }
QString NetworkControl::GetHotspotPassword() const { return hotspot_password_; }

// ---------------------------------------------------------------------------
// Operations
// ---------------------------------------------------------------------------

void NetworkControl::SetAirplaneMode(bool enabled) {
  QDBusInterface nm(DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_PROPERTIES_INTERFACE,
    bus_);
  nm.call("Set",
    QString(DBUS_NETWORK_MANAGER_INTERFACE),
    QStringLiteral("WirelessEnabled"),
    QVariant::fromValue(QDBusVariant(!enabled)));

  airplane_mode_ = enabled;
  emit AirplaneModeChanged();
  emit StatusChanged();
}

void NetworkControl::SelectWlanDevice(const QString& device_path) {
  if (device_path == selected_wlan_device_path_) return;
  selected_wlan_device_path_ = device_path;
  RefreshWlanData();
  emit WlanChanged();
}

void NetworkControl::SetWlanDeviceEnabled(bool enabled) {
  if (selected_wlan_device_path_.isEmpty()) return;

  QDBusInterface dev(DBUS_NETWORK_MANAGER_SERVICE,
    selected_wlan_device_path_,
    DBUS_NETWORK_MANAGER_DEVICE_INTERFACE,
    bus_);
  dev.call(enabled ? "SetAutoconnect" : "Disconnect");

  if (!enabled) {
    // Explicitly disconnect to make the device unavailable
    dev.call("Disconnect");
  }
  RefreshWlanData();
  emit WlanChanged();
}

void NetworkControl::ConnectWlan(const QString& ap_path,
    const QString& password) {
  if (selected_wlan_device_path_.isEmpty()) return;

  // Read AP properties
  QVariant ssid_var = GetNmProperty(
    ap_path, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE, "Ssid");
  QByteArray ssid = ssid_var.toByteArray();

  QVariant ap_flags_var = GetNmProperty(
    ap_path, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE, "Flags");
  QVariant wpa_var = GetNmProperty(
    ap_path, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE, "WpaFlags");
  QVariant rsn_var = GetNmProperty(
    ap_path, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE, "RsnFlags");

  bool secured = (ap_flags_var.toUInt() & NC_AP_FLAG_PRIVACY) != 0
              || wpa_var.toUInt() != 0
              || rsn_var.toUInt() != 0;

  QDBusInterface nm(DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_NETWORK_MANAGER_INTERFACE,
    bus_);

  // Try to find a saved connection first
  QString saved_conn = FindSavedConnection(ssid);
  if (!saved_conn.isEmpty()) {
    nm.call("ActivateConnection",
      QVariant::fromValue(QDBusObjectPath(saved_conn)),
      QVariant::fromValue(QDBusObjectPath(selected_wlan_device_path_)),
      QVariant::fromValue(QDBusObjectPath(ap_path)));
    pending_connect_ap_path_ = ap_path;
    return;
  }

  // No saved connection
  if (secured && password.isEmpty()) {
    QString category = SecurityCategoryFromFlags(
      ap_flags_var.toUInt(), wpa_var.toUInt(), rsn_var.toUInt());
    emit PasswordRequired(SsidToString(ssid), ap_path, category);
    return;
  }

  QVariantMap settings;
  if (secured) {
    settings = BuildWpaPskConnectionSettings(ssid, password);
  } else {
    settings = BuildOpenConnectionSettings(ssid);
  }

  // Register as NmSettingsMap for D-Bus marshalling
  NmSettingsMap nm_settings;
  for (auto it = settings.begin(); it != settings.end(); ++it) {
    nm_settings.insert(it.key(), it.value().toMap());
  }

  QDBusArgument arg;
  arg.beginMap(QVariant::String, qMetaTypeId<QVariantMap>());
  for (auto it = nm_settings.begin(); it != nm_settings.end(); ++it) {
    arg.beginMapEntry();
    arg << it.key() << it.value();
    arg.endMapEntry();
  }
  arg.endMap();

  nm.call("AddAndActivateConnection",
    QVariant::fromValue(arg),
    QVariant::fromValue(QDBusObjectPath(selected_wlan_device_path_)),
    QVariant::fromValue(QDBusObjectPath(ap_path)));
  pending_connect_ap_path_ = ap_path;
}

void NetworkControl::ConnectWlanAdvanced(const QString& ap_path,
    const QVariantMap& credentials) {
  if (selected_wlan_device_path_.isEmpty()) return;

  QVariant ssid_var = GetNmProperty(
    ap_path, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE, "Ssid");
  QByteArray ssid = ssid_var.toByteArray();

  QString type = credentials.value(QStringLiteral("type")).toString();
  QVariantMap settings;
  if (type == QLatin1String(NC_SEC_WEP)) {
    settings = BuildWepConnectionSettings(ssid, credentials);
  } else {
    settings = BuildEapConnectionSettings(ssid, credentials);
  }

  QDBusInterface nm(DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_NETWORK_MANAGER_INTERFACE,
    bus_);

  NmSettingsMap nm_settings;
  for (auto it = settings.begin(); it != settings.end(); ++it) {
    nm_settings.insert(it.key(), it.value().toMap());
  }

  QDBusArgument arg;
  arg.beginMap(QVariant::String, qMetaTypeId<QVariantMap>());
  for (auto it = nm_settings.begin(); it != nm_settings.end(); ++it) {
    arg.beginMapEntry();
    arg << it.key() << it.value();
    arg.endMapEntry();
  }
  arg.endMap();

  nm.call("AddAndActivateConnection",
    QVariant::fromValue(arg),
    QVariant::fromValue(QDBusObjectPath(selected_wlan_device_path_)),
    QVariant::fromValue(QDBusObjectPath(ap_path)));
  pending_connect_ap_path_ = ap_path;
}

void NetworkControl::DisconnectWlan(const QString& device_path) {
  if (device_path.isEmpty()) return;
  QDBusInterface dev(DBUS_NETWORK_MANAGER_SERVICE,
    device_path,
    DBUS_NETWORK_MANAGER_DEVICE_INTERFACE,
    bus_);
  dev.call("Disconnect");
}

void NetworkControl::DisconnectEthernet(const QString& device_path) {
  if (device_path.isEmpty()) return;
  QDBusInterface dev(DBUS_NETWORK_MANAGER_SERVICE,
    device_path,
    DBUS_NETWORK_MANAGER_DEVICE_INTERFACE,
    bus_);
  dev.call("Disconnect");
}

void NetworkControl::SetHotspotEnabled(bool enabled) {
  if (!hotspot_available_) return;

  // Find a WiFi device not used by the primary connection
  QString hotspot_dev;
  for (const QVariant& entry : wlan_devices_) {
    const QVariantMap dev = entry.toMap();
    if (dev.value("path").toString() != selected_wlan_device_path_) {
      hotspot_dev = dev.value("path").toString();
      break;
    }
  }
  if (hotspot_dev.isEmpty()) return;

  QDBusInterface nm(DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_NETWORK_MANAGER_INTERFACE,
    bus_);

  if (enabled) {
    QString name = hotspot_name_.isEmpty()
                 ? QStringLiteral("Hotspot") : hotspot_name_;
    QByteArray ssid = name.toUtf8();

    // Build AP-mode connection settings
    QVariantMap wireless;
    wireless[QStringLiteral("ssid")] = ssid;
    wireless[QStringLiteral("mode")] = QStringLiteral("ap");
    wireless[QStringLiteral("band")] = QStringLiteral("bg");

    QVariantMap security;
    security[QStringLiteral("key-mgmt")] = QStringLiteral("wpa-psk");
    security[QStringLiteral("psk")] = hotspot_password_;

    QVariantMap conn;
    conn[QStringLiteral("type")] = QStringLiteral("802-11-wireless");
    conn[QStringLiteral("id")] = name;
    conn[QStringLiteral("uuid")] = QUuid::createUuid().toString(
      QUuid::WithoutBraces);

    QVariantMap ipv4;
    ipv4[QStringLiteral("method")] = QStringLiteral("shared");

    QVariantMap ipv6;
    ipv6[QStringLiteral("method")] = QStringLiteral("ignore");

    NmSettingsMap nm_settings;
    nm_settings[QStringLiteral("connection")] = conn;
    nm_settings[QStringLiteral("802-11-wireless")] = wireless;
    nm_settings[QStringLiteral("802-11-wireless-security")] = security;
    nm_settings[QStringLiteral("ipv4")] = ipv4;
    nm_settings[QStringLiteral("ipv6")] = ipv6;

    QDBusArgument arg;
    arg.beginMap(QVariant::String, qMetaTypeId<QVariantMap>());
    for (auto it = nm_settings.begin(); it != nm_settings.end(); ++it) {
      arg.beginMapEntry();
      arg << it.key() << it.value();
      arg.endMapEntry();
    }
    arg.endMap();

    nm.call("AddAndActivateConnection",
      QVariant::fromValue(arg),
      QVariant::fromValue(QDBusObjectPath(hotspot_dev)),
      QVariant::fromValue(QDBusObjectPath(QStringLiteral("/"))));
  } else {
    // Disconnect the hotspot device
    QDBusInterface dev(DBUS_NETWORK_MANAGER_SERVICE,
      hotspot_dev,
      DBUS_NETWORK_MANAGER_DEVICE_INTERFACE,
      bus_);
    dev.call("Disconnect");
  }

  hotspot_enabled_ = enabled;
  emit HotspotChanged();
}

void NetworkControl::SetHotspotConfig(const QString& name,
    const QString& password) {
  hotspot_name_ = name;
  hotspot_password_ = password;
  emit HotspotChanged();
}

// static
QString NetworkControl::GetNetworkManagerDialogCommand() {
  // Try plasma-nm (KDE), then nm-connection-editor, then KCM fallback
  static const QStringList candidates = {
    QStringLiteral("plasma-nm"),
    QStringLiteral("nm-connection-editor"),
    QStringLiteral("kcmshell6 kcm_networkmanagement"),
    QStringLiteral("kcmshell5 kcm_networkmanagement"),
  };
  for (const QString& cmd : candidates) {
    QString bin = cmd.split(' ').first();
    QProcess probe;
    probe.start("which", {bin});
    probe.waitForFinished(500);
    if (probe.exitCode() == 0)
      return cmd;
  }
  return QStringLiteral("kcmshell6 kcm_networkmanagement");
}

// ---------------------------------------------------------------------------
// Internal refresh helpers
// ---------------------------------------------------------------------------

void NetworkControl::RefreshDevices() {
  QDBusInterface nm(DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_NETWORK_MANAGER_INTERFACE,
    bus_);
  if (!nm.isValid()) {
    LOG(ERROR) << absl::StrCat("NetworkControl: Cannot reach NetworkManager.");
    return;
  }

  QDBusReply<QList<QDBusObjectPath>> reply = nm.call("GetDevices");
  if (!reply.isValid()) {
    LOG(ERROR) << absl::StrCat("NetworkControl: GetDevices failed.");
    return;
  }

  wlan_devices_.clear();
  ethernet_devices_.clear();
  has_wlan_ = false;
  has_ethernet_ = false;

  for (const QString& path : monitored_device_paths_) {
    bus_.disconnect(DBUS_NETWORK_MANAGER_SERVICE, path,
      DBUS_PROPERTIES_INTERFACE, DBUS_PROP_UPDTE_METHOD,
      this,
      SLOT(OnDevicePropertiesChanged(QString, QVariantMap, QStringList)));
  }
  monitored_device_paths_.clear();

  for (const QDBusObjectPath& dp : reply.value()) {
    QVariant type = GetNmProperty(
      dp.path(), DBUS_NETWORK_MANAGER_DEVICE_INTERFACE, "DeviceType");
    QVariant device_name = GetNmProperty(
      dp.path(), DBUS_NETWORK_MANAGER_DEVICE_INTERFACE, "Interface");
    uint dtype = type.toUInt();

    if (dtype == NC_DEVICE_TYPE_WIFI) {
      has_wlan_ = true;
      QVariantMap entry;
      entry[QStringLiteral("name")] = device_name.toString();
      entry[QStringLiteral("path")] = dp.path();
      entry[QStringLiteral("enabled")] = true;
      wlan_devices_.append(entry);
      SubscribeToDeviceProperties(dp.path());
    } else if (dtype == NC_DEVICE_TYPE_ETHERNET) {
      has_ethernet_ = true;
      QVariantMap entry;
      entry[QStringLiteral("name")] = device_name.toString();
      entry[QStringLiteral("path")] = dp.path();
      SubscribeToDeviceProperties(dp.path());
      // Ethernet entries are fully built in RefreshEthernetData()
    }
  }

  // Default to first WiFi device if current selection is gone
  if (!selected_wlan_device_path_.isEmpty()) {
    bool still_present = false;
    for (const QVariant& d : wlan_devices_) {
      if (d.toMap().value("path").toString() == selected_wlan_device_path_) {
        still_present = true;
        break;
      }
    }
    if (!still_present) selected_wlan_device_path_.clear();
  }
  if (selected_wlan_device_path_.isEmpty() && !wlan_devices_.isEmpty())
    selected_wlan_device_path_ =
      wlan_devices_.first().toMap().value("path").toString();

  RefreshWlanData();
  RefreshEthernetData();
  RefreshConnectivity();
  RefreshHotspot();

  // Read airplane mode (inverse of WirelessEnabled)
  QVariant we = GetNmProperty(
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_NETWORK_MANAGER_INTERFACE, "WirelessEnabled");
  airplane_mode_ = !we.toBool();

  emit StatusChanged();
  emit WlanChanged();
  emit EthernetChanged();
  emit AirplaneModeChanged();
}

void NetworkControl::RefreshWlanData() {
  current_wlan_network_ = QVariantMap{};
  available_wlan_networks_.clear();

  if (selected_wlan_device_path_.isEmpty()) return;

  uint dev_state = GetNmProperty(
    selected_wlan_device_path_,
    DBUS_NETWORK_MANAGER_DEVICE_INTERFACE, "State").toUInt();

  // Update enabled flag in wlan_devices_
  bool dev_enabled = (dev_state != NC_DEVICE_STATE_UNAVAILABLE
                   && dev_state != NC_DEVICE_STATE_UNMANAGED);
  for (int i = 0; i < wlan_devices_.size(); ++i) {
    QVariantMap m = wlan_devices_[i].toMap();
    if (m.value("path").toString() == selected_wlan_device_path_) {
      m[QStringLiteral("enabled")] = dev_enabled;
      wlan_devices_[i] = m;
      break;
    }
  }

  // Get active access point
  QVariant active_ap_var = GetNmProperty(
    selected_wlan_device_path_,
    DBUS_NETWORK_MANAGER_WIRELESS_INTERFACE, "ActiveAccessPoint");
  QString active_ap = active_ap_var.value<QDBusObjectPath>().path();

  if (!active_ap.isEmpty() && active_ap != QLatin1String("/")) {
    QVariant ssid_var = GetNmProperty(
      active_ap, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE, "Ssid");
    QByteArray ssid = ssid_var.toByteArray();
    int strength = GetApStrength(active_ap);
    uint ap_flags = GetNmProperty(
      active_ap, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE,
      "Flags").toUInt();
    uint wpa_flags = GetNmProperty(
      active_ap, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE,
      "WpaFlags").toUInt();
    uint rsn_flags = GetNmProperty(
      active_ap, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE,
      "RsnFlags").toUInt();

    QString status;
    if (dev_state == NC_DEVICE_STATE_ACTIVATED) {
      if (login_required_) status = QStringLiteral("no_internet");
      else status = QString{};
    } else if (dev_state >= NC_DEVICE_STATE_PREPARE
            && dev_state < NC_DEVICE_STATE_ACTIVATED) {
      status = (dev_state == NC_DEVICE_STATE_NEED_AUTH)
             ? QStringLiteral("pending_auth")
             : QStringLiteral("connecting");
    } else if (dev_state == NC_DEVICE_STATE_FAILED) {
      status = QStringLiteral("failed");
    }

    current_wlan_network_[QStringLiteral("ssid")] = SsidToString(ssid);
    current_wlan_network_[QStringLiteral("strength")] = strength;
    current_wlan_network_[QStringLiteral("security")] =
      SecurityTypeFromFlags(ap_flags, wpa_flags, rsn_flags);
    current_wlan_network_[QStringLiteral("icon")] =
      WlanIconFromStrength(strength);
    current_wlan_network_[QStringLiteral("status")] = status;
    current_wlan_network_[QStringLiteral("isHotspot")] = false;
    current_wlan_network_[QStringLiteral("isUsb")] = false;
    current_wlan_network_[QStringLiteral("isBluetooth")] = false;
    current_wlan_network_[QStringLiteral("apPath")] = active_ap;
    current_wlan_network_[QStringLiteral("devicePath")] =
      selected_wlan_device_path_;
  }

  // Get all visible access points
  QDBusInterface wireless(DBUS_NETWORK_MANAGER_SERVICE,
    selected_wlan_device_path_,
    DBUS_NETWORK_MANAGER_WIRELESS_INTERFACE, bus_);
  QDBusReply<QList<QDBusObjectPath>> aps = wireless.call("GetAllAccessPoints");
  if (!aps.isValid()) return;

  // SSID → best entry seen so far
  QMap<QString, QVariantMap> best_per_ssid;

  for (const QDBusObjectPath& ap_obj : aps.value()) {
    QString ap_path = ap_obj.path();
    if (ap_path == active_ap) continue;  // already shown in Current Network

    QVariant ssid_var = GetNmProperty(
      ap_path, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE, "Ssid");
    QByteArray ssid = ssid_var.toByteArray();
    if (ssid.isEmpty()) continue;

    QString ssid_str = SsidToString(ssid);
    int strength = GetApStrength(ap_path);

    // Skip if we already have a stronger AP for this SSID
    auto it = best_per_ssid.find(ssid_str);
    if (it != best_per_ssid.end()
        && it->value("strength").toInt() >= strength)
      continue;

    uint ap_flags = GetNmProperty(
      ap_path, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE,
      "Flags").toUInt();
    uint wpa_flags = GetNmProperty(
      ap_path, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE,
      "WpaFlags").toUInt();
    uint rsn_flags = GetNmProperty(
      ap_path, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE,
      "RsnFlags").toUInt();

    QVariantMap entry;
    entry[QStringLiteral("ssid")] = ssid_str;
    entry[QStringLiteral("strength")] = strength;
    entry[QStringLiteral("security")] =
      SecurityTypeFromFlags(ap_flags, wpa_flags, rsn_flags);
    entry[QStringLiteral("icon")] = WlanIconFromStrength(strength);
    entry[QStringLiteral("apPath")] = ap_path;
    entry[QStringLiteral("status")] =
      (ap_path == pending_connect_ap_path_)
      ? QStringLiteral("connecting")
      : QString{};
    entry[QStringLiteral("isHotspot")] = false;
    entry[QStringLiteral("isUsb")] = false;
    entry[QStringLiteral("isBluetooth")] = false;
    best_per_ssid[ssid_str] = entry;
  }

  // Sort by signal strength descending and append to the list
  QList<QVariantMap> deduped = best_per_ssid.values();
  std::sort(deduped.begin(), deduped.end(),
    [](const QVariantMap& a, const QVariantMap& b) {
      return a.value("strength").toInt() > b.value("strength").toInt();
    });
  for (const QVariantMap& e : deduped)
    available_wlan_networks_.append(e);
}

void NetworkControl::RefreshEthernetData() {
  ethernet_devices_.clear();

  QDBusInterface nm(DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_NETWORK_MANAGER_INTERFACE, bus_);
  QDBusReply<QList<QDBusObjectPath>> reply = nm.call("GetDevices");
  if (!reply.isValid()) return;

  for (const QDBusObjectPath& dp : reply.value()) {
    uint dtype = GetNmProperty(
      dp.path(), DBUS_NETWORK_MANAGER_DEVICE_INTERFACE,
      "DeviceType").toUInt();
    if (dtype != NC_DEVICE_TYPE_ETHERNET) continue;

    QString device_name = GetNmProperty(
      dp.path(), DBUS_NETWORK_MANAGER_DEVICE_INTERFACE,
      "Interface").toString();
    uint state = GetNmProperty(
      dp.path(), DBUS_NETWORK_MANAGER_DEVICE_INTERFACE,
      "State").toUInt();

    bool carrier = GetNmProperty(
      dp.path(), DBUS_NETWORK_MANAGER_WIRED_INTERFACE,
      "Carrier").toBool();

    QString status;
    QString icon;
    bool is_connected = false;

    if (state == NC_DEVICE_STATE_ACTIVATED) {
      if (login_required_) {
        status = QStringLiteral("pending_authentication");
        icon = QStringLiteral("captive_portal");
      } else {
        status = QStringLiteral("connected");
        icon = QStringLiteral("settings_ethernet");
        is_connected = true;
      }
    } else if (!carrier || state == NC_DEVICE_STATE_UNAVAILABLE
            || state == NC_DEVICE_STATE_DISCONNECTED) {
      status = QStringLiteral("unplugged");
      icon = QStringLiteral("globe_2_cancel");
    } else if (state >= NC_DEVICE_STATE_PREPARE
            && state < NC_DEVICE_STATE_ACTIVATED) {
      status = (state == NC_DEVICE_STATE_NEED_AUTH)
             ? QStringLiteral("pending_authentication")
             : QStringLiteral("no_network");
      icon = QStringLiteral("settings_ethernet");
    } else {
      status = QStringLiteral("no_network");
      icon = QStringLiteral("globe_2_cancel");
    }

    QVariantMap entry;
    entry[QStringLiteral("name")] = device_name;
    entry[QStringLiteral("path")] = dp.path();
    entry[QStringLiteral("status")] = status;
    entry[QStringLiteral("icon")] = icon;
    entry[QStringLiteral("isHotspot")] = false;
    entry[QStringLiteral("isUsb")] = false;
    entry[QStringLiteral("isConnected")] = is_connected;
    ethernet_devices_.append(entry);
  }
}

void NetworkControl::RefreshConnectivity() {
  QVariant conn = GetNmProperty(
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_NETWORK_MANAGER_INTERFACE, "Connectivity");
  uint c = conn.toUInt();
  bool was = login_required_;
  login_required_ = (c == NC_CONNECTIVITY_PORTAL);
  if (login_required_ != was) emit StatusChanged();
}

void NetworkControl::RefreshHotspot() {
  // Hotspot is available when there are at least 2 WiFi devices, or
  // when there is 1 WiFi device (it can run in AP mode if supported).
  // We conservatively require 2 devices.
  hotspot_available_ = wlan_devices_.size() >= 2;
  hotspot_enabled_ = false;  // Re-detect if needed in the future
  emit HotspotChanged();
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void NetworkControl::SubscribeToDeviceProperties(const QString& device_path) {
  if (monitored_device_paths_.contains(device_path)) return;
  monitored_device_paths_.append(device_path);
  bus_.connect(
    DBUS_NETWORK_MANAGER_SERVICE,
    device_path,
    DBUS_PROPERTIES_INTERFACE,
    DBUS_PROP_UPDTE_METHOD,
    this,
    SLOT(OnDevicePropertiesChanged(QString, QVariantMap, QStringList)));
}

QString NetworkControl::FindSavedConnection(const QByteArray& ssid) const {
  QDBusInterface settings(DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_SETTINGS_PATH,
    DBUS_NETWORK_MANAGER_SETTINGS_INTERFACE,
    bus_);
  QDBusReply<QList<QDBusObjectPath>> reply = settings.call("ListConnections");
  if (!reply.isValid()) return {};

  for (const QDBusObjectPath& cp : reply.value()) {
    QDBusInterface conn(DBUS_NETWORK_MANAGER_SERVICE,
      cp.path(),
      DBUS_NETWORK_MANAGER_SETTINGS_CONNECTION_INTERFACE,
      bus_);
    QDBusReply<QVariantMap> settings_reply = conn.call("GetSettings");
    if (!settings_reply.isValid()) continue;

    const QVariantMap& all = settings_reply.value();
    QVariantMap wireless = all.value(
      QStringLiteral("802-11-wireless")).toMap();
    QByteArray stored_ssid = wireless.value(
      QStringLiteral("ssid")).toByteArray();
    if (stored_ssid == ssid) return cp.path();
  }
  return {};
}

// static
QVariantMap NetworkControl::BuildWpaPskConnectionSettings(
    const QByteArray& ssid, const QString& password) {
  QVariantMap conn;
  conn[QStringLiteral("type")] = QStringLiteral("802-11-wireless");
  conn[QStringLiteral("id")] = QString::fromUtf8(ssid);
  conn[QStringLiteral("uuid")] = QUuid::createUuid().toString(
    QUuid::WithoutBraces);

  QVariantMap wireless;
  wireless[QStringLiteral("ssid")] = ssid;
  wireless[QStringLiteral("mode")] = QStringLiteral("infrastructure");

  QVariantMap security;
  security[QStringLiteral("key-mgmt")] = QStringLiteral("wpa-psk");
  security[QStringLiteral("psk")] = password;

  QVariantMap ipv4;
  ipv4[QStringLiteral("method")] = QStringLiteral("auto");

  QVariantMap ipv6;
  ipv6[QStringLiteral("method")] = QStringLiteral("auto");

  QVariantMap result;
  result[QStringLiteral("connection")] = conn;
  result[QStringLiteral("802-11-wireless")] = wireless;
  result[QStringLiteral("802-11-wireless-security")] = security;
  result[QStringLiteral("ipv4")] = ipv4;
  result[QStringLiteral("ipv6")] = ipv6;
  return result;
}

// static
QVariantMap NetworkControl::BuildOpenConnectionSettings(
    const QByteArray& ssid) {
  QVariantMap conn;
  conn[QStringLiteral("type")] = QStringLiteral("802-11-wireless");
  conn[QStringLiteral("id")] = QString::fromUtf8(ssid);
  conn[QStringLiteral("uuid")] = QUuid::createUuid().toString(
    QUuid::WithoutBraces);

  QVariantMap wireless;
  wireless[QStringLiteral("ssid")] = ssid;
  wireless[QStringLiteral("mode")] = QStringLiteral("infrastructure");

  QVariantMap ipv4;
  ipv4[QStringLiteral("method")] = QStringLiteral("auto");

  QVariantMap ipv6;
  ipv6[QStringLiteral("method")] = QStringLiteral("auto");

  QVariantMap result;
  result[QStringLiteral("connection")] = conn;
  result[QStringLiteral("802-11-wireless")] = wireless;
  result[QStringLiteral("ipv4")] = ipv4;
  result[QStringLiteral("ipv6")] = ipv6;
  return result;
}

// static
QVariantMap NetworkControl::BuildWepConnectionSettings(const QByteArray& ssid,
    const QVariantMap& credentials) {
  QVariantMap conn;
  conn[QStringLiteral("type")] = QStringLiteral("802-11-wireless");
  conn[QStringLiteral("id")] = QString::fromUtf8(ssid);
  conn[QStringLiteral("uuid")] = QUuid::createUuid().toString(
    QUuid::WithoutBraces);

  QVariantMap wireless;
  wireless[QStringLiteral("ssid")] = ssid;
  wireless[QStringLiteral("mode")] = QStringLiteral("infrastructure");

  QVariantMap security;
  security[QStringLiteral("key-mgmt")] = QStringLiteral("none");
  security[QStringLiteral("wep-key0")] =
    credentials.value(QStringLiteral("password")).toString();
  security[QStringLiteral("wep-tx-keyidx")] =
    credentials.value(QStringLiteral("wep_key_idx"), 0).toInt();
  security[QStringLiteral("auth-alg")] =
    credentials.value(QStringLiteral("auth_alg"),
      QStringLiteral("open")).toString();

  QVariantMap ipv4;
  ipv4[QStringLiteral("method")] = QStringLiteral("auto");

  QVariantMap ipv6;
  ipv6[QStringLiteral("method")] = QStringLiteral("auto");

  QVariantMap result;
  result[QStringLiteral("connection")] = conn;
  result[QStringLiteral("802-11-wireless")] = wireless;
  result[QStringLiteral("802-11-wireless-security")] = security;
  result[QStringLiteral("ipv4")] = ipv4;
  result[QStringLiteral("ipv6")] = ipv6;
  return result;
}

// static
QVariantMap NetworkControl::BuildEapConnectionSettings(const QByteArray& ssid,
    const QVariantMap& credentials) {
  QString method =
    credentials.value(QStringLiteral("eap_method")).toString().toLower();

  QVariantMap conn;
  conn[QStringLiteral("type")] = QStringLiteral("802-11-wireless");
  conn[QStringLiteral("id")] = QString::fromUtf8(ssid);
  conn[QStringLiteral("uuid")] = QUuid::createUuid().toString(
    QUuid::WithoutBraces);

  QVariantMap wireless;
  wireless[QStringLiteral("ssid")] = ssid;
  wireless[QStringLiteral("mode")] = QStringLiteral("infrastructure");

  QVariantMap security;
  security[QStringLiteral("key-mgmt")] = QStringLiteral("wpa-eap");

  QVariantMap eap;

  if (method == QLatin1String("tls")) {
    eap[QStringLiteral("eap")] =
      QStringList{QStringLiteral("tls")};
    eap[QStringLiteral("identity")] =
      credentials.value(QStringLiteral("identity")).toString();
    eap[QStringLiteral("domain-suffix-match")] =
      credentials.value(QStringLiteral("domain")).toString();
    eap[QStringLiteral("subject-match")] =
      credentials.value(QStringLiteral("subject_match")).toString();
    QString alt = credentials.value(
      QStringLiteral("alt_subject_match")).toString();
    if (!alt.isEmpty())
      eap[QStringLiteral("altsubject-matches")] = QStringList{alt};
    eap[QStringLiteral("client-cert")] = PathToNmCertBytes(
      credentials.value(QStringLiteral("user_cert")).toString());
    eap[QStringLiteral("ca-cert")] = PathToNmCertBytes(
      credentials.value(QStringLiteral("ca_cert")).toString());
    eap[QStringLiteral("private-key")] = PathToNmCertBytes(
      credentials.value(QStringLiteral("private_key")).toString());
    eap[QStringLiteral("private-key-password")] =
      credentials.value(QStringLiteral("private_key_password")).toString();
    eap[QStringLiteral("private-key-password-flags")] = 0;
  } else if (method == QLatin1String("leap")
        || method == QLatin1String("pwd")) {
    eap[QStringLiteral("eap")] = QStringList{method};
    eap[QStringLiteral("identity")] =
      credentials.value(QStringLiteral("username")).toString();
    eap[QStringLiteral("password")] =
      credentials.value(QStringLiteral("password")).toString();
    eap[QStringLiteral("password-flags")] = 0;
  } else if (method == QLatin1String("fast")) {
    eap[QStringLiteral("eap")] =
      QStringList{QStringLiteral("fast")};
    eap[QStringLiteral("anonymous-identity")] =
      credentials.value(QStringLiteral("anon_identity")).toString();
    eap[QStringLiteral("phase1-fast-provisioning")] =
      credentials.value(QStringLiteral("pac_provisioning"),
        QStringLiteral("0")).toString();
    QString pac = credentials.value(QStringLiteral("pac_file")).toString();
    if (!pac.isEmpty())
      eap[QStringLiteral("pac-file")] = PathToNmCertBytes(pac);
    eap[QStringLiteral("phase2-auth")] =
      credentials.value(QStringLiteral("inner_auth"),
        QStringLiteral("gtc")).toString();
    eap[QStringLiteral("identity")] =
      credentials.value(QStringLiteral("username")).toString();
    eap[QStringLiteral("password")] =
      credentials.value(QStringLiteral("password")).toString();
    eap[QStringLiteral("password-flags")] = 0;
  } else if (method == QLatin1String("ttls")) {
    eap[QStringLiteral("eap")] =
      QStringList{QStringLiteral("ttls")};
    eap[QStringLiteral("anonymous-identity")] =
      credentials.value(QStringLiteral("anon_identity")).toString();
    eap[QStringLiteral("domain-suffix-match")] =
      credentials.value(QStringLiteral("domain")).toString();
    eap[QStringLiteral("ca-cert")] = PathToNmCertBytes(
      credentials.value(QStringLiteral("ca_cert")).toString());
    eap[QStringLiteral("phase2-auth")] =
      credentials.value(QStringLiteral("inner_auth"),
        QStringLiteral("pap")).toString();
    eap[QStringLiteral("identity")] =
      credentials.value(QStringLiteral("username")).toString();
    eap[QStringLiteral("password")] =
      credentials.value(QStringLiteral("password")).toString();
    eap[QStringLiteral("password-flags")] = 0;
  } else {
    // Default: PEAP
    eap[QStringLiteral("eap")] =
      QStringList{QStringLiteral("peap")};
    eap[QStringLiteral("anonymous-identity")] =
      credentials.value(QStringLiteral("anon_identity")).toString();
    eap[QStringLiteral("domain-suffix-match")] =
      credentials.value(QStringLiteral("domain")).toString();
    eap[QStringLiteral("ca-cert")] = PathToNmCertBytes(
      credentials.value(QStringLiteral("ca_cert")).toString());
    QString peap_ver =
      credentials.value(QStringLiteral("peap_version")).toString();
    if (peap_ver == QLatin1String("0") || peap_ver == QLatin1String("1"))
      eap[QStringLiteral("phase1-peapver")] = peap_ver;
    eap[QStringLiteral("phase2-auth")] =
      credentials.value(QStringLiteral("inner_auth"),
        QStringLiteral("mschapv2")).toString();
    eap[QStringLiteral("identity")] =
      credentials.value(QStringLiteral("username")).toString();
    eap[QStringLiteral("password")] =
      credentials.value(QStringLiteral("password")).toString();
    eap[QStringLiteral("password-flags")] = 0;
  }

  QVariantMap ipv4;
  ipv4[QStringLiteral("method")] = QStringLiteral("auto");

  QVariantMap ipv6;
  ipv6[QStringLiteral("method")] = QStringLiteral("auto");

  QVariantMap result;
  result[QStringLiteral("connection")] = conn;
  result[QStringLiteral("802-11-wireless")] = wireless;
  result[QStringLiteral("802-11-wireless-security")] = security;
  result[QStringLiteral("802-1x")] = eap;
  result[QStringLiteral("ipv4")] = ipv4;
  result[QStringLiteral("ipv6")] = ipv6;
  return result;
}

// static
QString NetworkControl::SecurityCategoryFromFlags(
    uint ap_flags, uint wpa_flags, uint rsn_flags) {
  bool enterprise_wpa = (wpa_flags & NC_AP_SEC_KEY_MGMT_8021X) != 0;
  bool enterprise_rsn = (rsn_flags & NC_AP_SEC_KEY_MGMT_8021X) != 0;
  if (enterprise_wpa || enterprise_rsn)
    return QStringLiteral(NC_SEC_ENTERPRISE);

  bool has_psk = (wpa_flags & NC_AP_SEC_KEY_MGMT_PSK) != 0
              || (rsn_flags & NC_AP_SEC_KEY_MGMT_PSK) != 0
              || (rsn_flags & NC_AP_SEC_KEY_MGMT_SAE) != 0;
  if (has_psk)
    return QStringLiteral(NC_SEC_WPA_PERSONAL);

  if ((ap_flags & NC_AP_FLAG_PRIVACY) != 0
      && wpa_flags == 0 && rsn_flags == 0)
    return QStringLiteral(NC_SEC_WEP);

  return QStringLiteral(NC_SEC_WPA_PERSONAL);
}

// static
QByteArray NetworkControl::PathToNmCertBytes(const QString& path) {
  if (path.isEmpty()) return {};
  QByteArray result = QByteArrayLiteral("file://");
  result += path.toUtf8();
  result += '\0';
  return result;
}

// static
QString NetworkControl::SecurityTypeFromFlags(
    uint ap_flags, uint wpa_flags, uint rsn_flags) {
  if (!(ap_flags & NC_AP_FLAG_PRIVACY) && wpa_flags == 0 && rsn_flags == 0)
    return QStringLiteral("None");

  bool has_wpa = (wpa_flags != 0);
  bool has_wpa2 = (rsn_flags != 0);
  bool sae = (rsn_flags & NC_AP_SEC_KEY_MGMT_SAE) != 0;
  bool owe = (rsn_flags & NC_AP_SEC_KEY_MGMT_OWE) != 0;
  bool enterprise_wpa = (wpa_flags & NC_AP_SEC_KEY_MGMT_8021X) != 0;
  bool enterprise_wpa2 = (rsn_flags & NC_AP_SEC_KEY_MGMT_8021X) != 0;

  if (owe)
    return QStringLiteral("Opportunistic Wireless Encryption");
  if (sae)
    return QStringLiteral("WPA3 Personal");
  if (enterprise_wpa2)
    return QStringLiteral("WPA/WPA2 Enterprise");
  if (enterprise_wpa)
    return QStringLiteral("WPA/WPA2 Enterprise");
  if (has_wpa && has_wpa2)
    return QStringLiteral("WPA/WPA2 Personal");
  if (has_wpa2)
    return QStringLiteral("WPA/WPA2 Personal");
  if (has_wpa)
    return QStringLiteral("WPA/WPA2 Personal");

  // Privacy flag set but no WPA flags — assume WEP
  return QStringLiteral("WEP40/128 w/ HEX or ASCII");
}

// static
QString NetworkControl::WlanIconFromStrength(int strength) {
  switch (strength) {
    case -1: return QStringLiteral("signal_wifi_off");
    case 0 ... 10: return QStringLiteral("signal_wifi_0_bar");
    case 11 ... 25: return QStringLiteral("network_wifi_1_bar");
    case 26 ... 50: return QStringLiteral("network_wifi_2_bar");
    case 51 ... 75: return QStringLiteral("network_wifi");
    case 76 ... 100: return QStringLiteral("signal_wifi_4_bar");
    default: return QStringLiteral("wifi_find");
  }
}

// static
int NetworkControl::GetApStrength(const QString& ap_path) {
  QVariant v = GetNmProperty(
    ap_path, DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE, "Strength");
  if (!v.isValid()) return -1;
  return v.toInt();
}

// static
QString NetworkControl::SsidToString(const QByteArray& ssid) {
  return QString::fromUtf8(ssid);
}

// static
QVariant NetworkControl::GetNmProperty(const QString& path,
    const QString& nm_interface,
    const QString& prop) {
  QDBusInterface props(DBUS_NETWORK_MANAGER_SERVICE,
    path,
    DBUS_PROPERTIES_INTERFACE,
    QDBusConnection::systemBus());
  QDBusReply<QVariant> reply = props.call("Get", nm_interface, prop);
  if (!reply.isValid()) return {};
  return reply.value();
}

// D-Bus slots
void NetworkControl::OnNmPropertiesChanged(const QString& nm_interface,
    const QVariantMap& changed,
    const QStringList& /*invalidated*/) {
  if (nm_interface != QLatin1String(DBUS_NETWORK_MANAGER_INTERFACE)) return;

  if (changed.contains(QStringLiteral("WirelessEnabled"))) {
    airplane_mode_ = !changed.value(
      QStringLiteral("WirelessEnabled")).toBool();
    emit AirplaneModeChanged();
    emit StatusChanged();
  }

  if (changed.contains(QStringLiteral("Connectivity"))) {
    uint c = changed.value(QStringLiteral("Connectivity")).toUInt();
    bool was = login_required_;
    login_required_ = (c == NC_CONNECTIVITY_PORTAL);
    if (login_required_ != was) {
      RefreshWlanData();
      RefreshEthernetData();
      emit StatusChanged();
      emit WlanChanged();
      emit EthernetChanged();
    }
  }
}

void NetworkControl::OnDevicePropertiesChanged(
    const QString& /*nm_interface*/,
    const QVariantMap& /*changed*/,
    const QStringList& /*invalidated*/) {
  // Any device property change may affect icon / status — do a targeted refresh
  RefreshWlanData();
  RefreshEthernetData();
  RefreshConnectivity();
  emit StatusChanged();
  emit WlanChanged();
  emit EthernetChanged();
}

void NetworkControl::OnNmStateChanged(uint /*state*/) {
  RefreshWlanData();
  RefreshEthernetData();
  RefreshConnectivity();
  emit StatusChanged();
  emit WlanChanged();
  emit EthernetChanged();
}

void NetworkControl::OnDeviceAdded(const QDBusObjectPath& /*device_path*/) {
  RefreshDevices();
}

void NetworkControl::OnDeviceRemoved(const QDBusObjectPath& /*device_path*/) {
  RefreshDevices();
}

}  // namespace backend
}  // namespace panel
