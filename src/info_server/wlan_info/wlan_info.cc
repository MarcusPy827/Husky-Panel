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

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QList>
#include <QDebug>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/wlan_info/wlan_info.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

WlanInfo::WlanInfo(QObject* parent) : QObject(parent) {
  FindAndSubscribeToWifiDevice();

  QDBusConnection::systemBus().connect(
    DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH,
    DBUS_NETWORK_MANAGER_INTERFACE,
    "StateChanged",
    this, SLOT(OnNmStateChanged(uint)));
}

void WlanInfo::FindAndSubscribeToWifiDevice() {
  QDBusInterface nm_iface(DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH, DBUS_NETWORK_MANAGER_INTERFACE,
    QDBusConnection::systemBus());

  if (!nm_iface.isValid()) {
    LOG(ERROR) << absl::StrCat("WlanInfo: Failed to connect to NetworkManager.");
    return;
  }

  QDBusReply<QList<QDBusObjectPath>> reply = nm_iface.call("GetDevices");
  if (!reply.isValid()) {
    LOG(ERROR) << absl::StrCat("WlanInfo: Cannot enumerate devices.");
    return;
  }

  for (const QDBusObjectPath& device_path : reply.value()) {
    QDBusInterface device_iface(DBUS_NETWORK_MANAGER_SERVICE,
      device_path.path(), DBUS_NETWORK_MANAGER_DEVICE_INTERFACE,
      QDBusConnection::systemBus());
    QVariant type_var = device_iface.property("DeviceType");
    if (type_var.isValid() && type_var.toUInt() == WI_WIFI_TYPE) {
      wifi_device_path_ = device_path.path();
      break;
    }
  }

  if (wifi_device_path_.isEmpty()) {
    LOG(WARNING) << absl::StrCat("WlanInfo: No Wi-Fi device found.");
    return;
  }

  QDBusConnection::systemBus().connect(
    DBUS_NETWORK_MANAGER_SERVICE,
    wifi_device_path_,
    DBUS_PROPERTIES_INTERFACE,
    DBUS_PROP_UPDTE_METHOD,
    this,
    SLOT(OnDevicePropertiesChanged(QString, QVariantMap, QStringList)));

  QDBusInterface wireless_iface(DBUS_NETWORK_MANAGER_SERVICE,
    wifi_device_path_, DBUS_NETWORK_MANAGER_WIRELESS_INTERFACE,
    QDBusConnection::systemBus());
  QString ap_path =
    wireless_iface.property("ActiveAccessPoint")
      .value<QDBusObjectPath>().path();
  if (!ap_path.isEmpty() && ap_path != "/") {
    active_ap_path_ = ap_path;
    SubscribeToAp(active_ap_path_);
  }
}

void WlanInfo::SubscribeToAp(const QString& ap_path) {
  QDBusConnection::systemBus().connect(
    DBUS_NETWORK_MANAGER_SERVICE,
    ap_path,
    DBUS_PROPERTIES_INTERFACE,
    DBUS_PROP_UPDTE_METHOD,
    this,
    SLOT(OnApPropertiesChanged(QString, QVariantMap, QStringList)));
}

void WlanInfo::UnsubscribeFromAp(const QString& ap_path) {
  QDBusConnection::systemBus().disconnect(
    DBUS_NETWORK_MANAGER_SERVICE,
    ap_path,
    DBUS_PROPERTIES_INTERFACE,
    DBUS_PROP_UPDTE_METHOD,
    this,
    SLOT(OnApPropertiesChanged(QString, QVariantMap, QStringList)));
}

void WlanInfo::OnDevicePropertiesChanged(const QString& interface,
    const QVariantMap& changed, const QStringList& invalidated) {
  Q_UNUSED(changed);
  Q_UNUSED(invalidated);

  if (interface != DBUS_NETWORK_MANAGER_WIRELESS_INTERFACE) {
    return;
  }

  QDBusInterface wireless_iface(DBUS_NETWORK_MANAGER_SERVICE,
    wifi_device_path_, DBUS_NETWORK_MANAGER_WIRELESS_INTERFACE,
    QDBusConnection::systemBus());
  QString new_ap =
    wireless_iface.property("ActiveAccessPoint")
      .value<QDBusObjectPath>().path();

  if (new_ap == active_ap_path_) {
    return;
  }

  if (!active_ap_path_.isEmpty()) {
    UnsubscribeFromAp(active_ap_path_);
  }

  active_ap_path_ = (new_ap != "/" && !new_ap.isEmpty()) ? new_ap : QString();
  if (!active_ap_path_.isEmpty()) {
    SubscribeToAp(active_ap_path_);
  }

  emit WlanStatusChanged();
}

void WlanInfo::OnApPropertiesChanged(const QString& interface,
    const QVariantMap& changed, const QStringList& invalidated) {
  Q_UNUSED(invalidated);

  if (interface != DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE) {
    return;
  }

  if (changed.contains("Strength")) {
    emit WlanStatusChanged();
  }
}

void WlanInfo::OnNmStateChanged(uint state) {
  Q_UNUSED(state);
  emit WlanStatusChanged();
}

int WlanInfo::GetWlanSignalStrength() {
  QDBusInterface interface(DBUS_NETWORK_MANAGER_SERVICE,
    DBUS_NETWORK_MANAGER_PATH, DBUS_NETWORK_MANAGER_INTERFACE,
    QDBusConnection::systemBus());

  if (!interface.isValid()) {
    qCritical() << "[ERROR] WLAN Info: Failed to connect to NetworkManager"
      << "D-Bus interface.";
    return -1;
  }

  QDBusReply<QList<QDBusObjectPath>> reply = interface.call("GetDevices");
  if (!reply.isValid()) {
    qCritical() << "[ERROR] WLAN Info: Cannot get WLAN device status."
      << reply.error().message();
    return -1;
  }

  QList<QDBusObjectPath> devices = reply.value();
  for (const QDBusObjectPath &device_path : devices) {
    QDBusInterface device_type_interface(DBUS_NETWORK_MANAGER_SERVICE,
      device_path.path(), DBUS_NETWORK_MANAGER_DEVICE_INTERFACE,
      QDBusConnection::systemBus());
    QVariant type_var = device_type_interface.property("DeviceType");

    if (type_var.isValid() && type_var.toUInt() == WI_WIFI_TYPE) {
      QDBusInterface wlan_interface(DBUS_NETWORK_MANAGER_SERVICE,
        device_path.path(), DBUS_NETWORK_MANAGER_WIRELESS_INTERFACE,
        QDBusConnection::systemBus());

      QVariant ap_path_raw = wlan_interface.property("ActiveAccessPoint");
      QDBusObjectPath ap_path = ap_path_raw.value<QDBusObjectPath>();

      if (ap_path.path() == "/") {
        return -1;
      }

      QDBusInterface ap_interface(DBUS_NETWORK_MANAGER_SERVICE, ap_path.path(),
        DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE,
        QDBusConnection::systemBus());

      QVariant strength_raw = ap_interface.property("Strength");
      if (strength_raw.isValid()) {
        return strength_raw.toInt();
      }
    }
  }

  return -1;
}

}  // namespace backend
}  // namespace panel
