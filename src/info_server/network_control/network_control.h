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

#ifndef SRC_INFO_SERVER_NETWORK_CONTROL_NETWORK_CONTROL_H_
#define SRC_INFO_SERVER_NETWORK_CONTROL_NETWORK_CONTROL_H_

// NM device type constants
#define NC_DEVICE_TYPE_ETHERNET 1
#define NC_DEVICE_TYPE_WIFI     2

// NM device state constants
#define NC_DEVICE_STATE_UNKNOWN        0
#define NC_DEVICE_STATE_UNMANAGED     10
#define NC_DEVICE_STATE_UNAVAILABLE   20
#define NC_DEVICE_STATE_DISCONNECTED  30
#define NC_DEVICE_STATE_PREPARE       40
#define NC_DEVICE_STATE_CONFIG        50
#define NC_DEVICE_STATE_NEED_AUTH     60
#define NC_DEVICE_STATE_IP_CONFIG     70
#define NC_DEVICE_STATE_IP_CHECK      80
#define NC_DEVICE_STATE_SECONDARIES   90
#define NC_DEVICE_STATE_ACTIVATED    100
#define NC_DEVICE_STATE_DEACTIVATING 110
#define NC_DEVICE_STATE_FAILED       120

// NM connectivity constants
#define NC_CONNECTIVITY_UNKNOWN  0
#define NC_CONNECTIVITY_NONE     1
#define NC_CONNECTIVITY_PORTAL   2
#define NC_CONNECTIVITY_LIMITED  3
#define NC_CONNECTIVITY_FULL     4

// AP security flags (WpaFlags / RsnFlags)
#define NC_AP_SEC_NONE         0x000
#define NC_AP_SEC_KEY_MGMT_PSK 0x100
#define NC_AP_SEC_KEY_MGMT_8021X 0x200
#define NC_AP_SEC_KEY_MGMT_SAE 0x400
#define NC_AP_SEC_KEY_MGMT_OWE 0x2000

// AP privacy flag
#define NC_AP_FLAG_PRIVACY 0x1

// Security category strings emitted with PasswordRequired
#define NC_SEC_WEP          "wep"
#define NC_SEC_WPA_PERSONAL "wpa_personal"
#define NC_SEC_ENTERPRISE   "enterprise"

#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

namespace panel {
namespace backend {

/**
 * @brief Monitors and controls network interfaces via NetworkManager D-Bus API.
 *
 * @details Exposes Wi-Fi, Ethernet, and hotspot data and supports
 *          connect/disconnect operations. All D-Bus callbacks are connected
 *          on the system bus; data mutations always happen on the Qt main
 *          thread via @c QueuedConnection or direct slot calls.
 */

class NetworkControl : public QObject {
  Q_OBJECT

 public:
  explicit NetworkControl(QObject* parent = nullptr);
  ~NetworkControl() override = default;

  // ---- Status bar icon helpers ----

  /**
   * @brief Returns @c true when at least one Wi-Fi device is present.
   * @return bool.
   */
  bool HasWlanHardware() const;

  /**
   * @brief Returns @c true when at least one Ethernet device is present.
   * @return bool.
   */
  bool HasEthernetHardware() const;

  /**
   * @brief Returns @c true when captive-portal connectivity is detected.
   * @return bool.
   */
  bool IsLoginRequired() const;

  /**
   * @brief Returns @c true when the wireless radio is disabled.
   * @return bool.
   */
  bool IsAirplaneModeEnabled() const;

  /**
   * @brief Returns the Material Symbol icon name for the Ethernet status.
   *
   * @details Returns @c "settings_ethernet" when any Ethernet device is
   *          connected, otherwise @c "globe_2_cancel".
   * @return QString.
   */
  QString GetEthernetIcon() const;

  /**
   * @brief Returns a Material Symbol icon name for the strongest active
   *        Wi-Fi signal.
   * @return QString.
   */
  QString GetWlanIcon() const;

  // ---- Wireless ----

  /**
   * @brief Returns the list of known Wi-Fi devices.
   *
   * @details Each entry is a map with keys @c name (QString), @c path
   *          (QString), and @c enabled (bool).
   * @return QVariantList.
   */
  QVariantList GetWlanDevices() const;

  /**
   * @brief Returns the D-Bus object path of the selected Wi-Fi device.
   * @return QString.
   */
  QString GetSelectedWlanDevice() const;

  /**
   * @brief Returns @c true when the selected Wi-Fi device is not in an
   *        unavailable or unmanaged state.
   * @return bool.
   */
  bool IsSelectedWlanDeviceEnabled() const;

  /**
   * @brief Returns the currently active Wi-Fi network properties.
   *
   * @details Map keys: @c ssid, @c strength, @c security, @c icon,
   *          @c status, @c isHotspot, @c isUsb, @c isBluetooth,
   *          @c apPath, @c devicePath.
   * @return QVariantMap.
   */
  QVariantMap GetCurrentWlanNetwork() const;

  /**
   * @brief Returns visible Wi-Fi access points excluding the active one.
   *
   * @details Each entry is a map with keys @c ssid, @c strength,
   *          @c security, @c apPath, @c icon, @c status, @c isHotspot,
   *          @c isUsb, @c isBluetooth.
   * @return QVariantList.
   */
  QVariantList GetAvailableWlanNetworks() const;

  // ---- Ethernet ----

  /**
   * @brief Returns the list of known Ethernet devices.
   *
   * @details Each entry is a map with keys @c name, @c path, @c status,
   *          @c icon, @c isHotspot, @c isUsb, @c isConnected.
   * @return QVariantList.
   */
  QVariantList GetEthernetDevices() const;

  // ---- Hotspot ----

  /**
   * @brief Returns @c true when the hotspot is currently active.
   * @return bool.
   */
  bool IsHotspotEnabled() const;

  /**
   * @brief Returns @c true when hotspot creation is supported.
   *
   * @details Requires at least two Wi-Fi devices to be present.
   * @return bool.
   */
  bool IsHotspotAvailable() const;

  /**
   * @brief Returns the configured hotspot SSID.
   * @return QString.
   */
  QString GetHotspotName() const;

  /**
   * @brief Returns the configured hotspot passphrase.
   * @return QString.
   */
  QString GetHotspotPassword() const;

  // ---- Operations (called from the Qt main thread) ----

  /**
   * @brief Enables or disables the wireless radio (airplane mode).
   *
   * @param enabled (bool) Pass @c true to enable airplane mode.
   * @return void.
   */
  void SetAirplaneMode(bool enabled);

  /**
   * @brief Switches the active Wi-Fi device to @p device_path.
   *
   * @param device_path (const QString&) D-Bus object path of the device.
   * @return void.
   */
  void SelectWlanDevice(const QString& device_path);

  /**
   * @brief Enables or disables the selected Wi-Fi device.
   *
   * @param enabled (bool) Pass @c false to call @c Disconnect on the device.
   * @return void.
   */
  void SetWlanDeviceEnabled(bool enabled);

  /**
   * @brief Connects to the access point at @p ap_path.
   *
   * @details For open networks: connects directly. For secured networks with
   *          no saved profile and an empty @p password, emits
   *          @c PasswordRequired with a security category string. For secured
   *          networks with an existing profile, activates it directly.
   * @param ap_path (const QString&) D-Bus object path of the access point.
   * @param password (const QString&) WPA pre-shared key, or empty string.
   * @return void.
   */
  void ConnectWlan(const QString& ap_path, const QString& password);

  /**
   * @brief Connects using explicit credentials for WEP or 802.1X networks.
   *
   * @details The @p credentials map must contain a @c "type" key set to
   *          @c "wep" or @c "enterprise". For WEP: @c "password",
   *          @c "wep_key_idx" (int, 0-based), @c "auth_alg"
   *          (@c "open"|@c "shared"). For enterprise: @c "eap_method"
   *          (@c "tls", @c "leap", @c "pwd", @c "fast", @c "ttls",
   *          @c "peap") plus method-specific keys (see implementation).
   * @param ap_path (const QString&) D-Bus object path of the access point.
   * @param credentials (const QVariantMap&) Credential map.
   * @return void.
   */
  void ConnectWlanAdvanced(const QString& ap_path,
      const QVariantMap& credentials);

  /**
   * @brief Calls @c Disconnect on the Wi-Fi device at @p device_path.
   *
   * @param device_path (const QString&) D-Bus object path of the device.
   * @return void.
   */
  void DisconnectWlan(const QString& device_path);

  /**
   * @brief Calls @c Disconnect on the Ethernet device at @p device_path.
   *
   * @param device_path (const QString&) D-Bus object path of the device.
   * @return void.
   */
  void DisconnectEthernet(const QString& device_path);

  /**
   * @brief Enables or disables the hotspot on a secondary Wi-Fi device.
   *
   * @param enabled (bool) Pass @c true to activate the hotspot.
   * @return void.
   */
  void SetHotspotEnabled(bool enabled);

  /**
   * @brief Stores the hotspot SSID and passphrase for the next activation.
   *
   * @param name (const QString&) Desired hotspot SSID.
   * @param password (const QString&) Desired WPA passphrase.
   * @return void.
   */
  void SetHotspotConfig(const QString& name, const QString& password);

  /**
   * @brief Returns a shell command that opens the system network manager
   *        dialog.
   *
   * @details Tries @c plasma-nm, then @c nm-connection-editor, then KCM
   *          shell as a fallback.
   * @return QString.
   */
  static QString GetNetworkManagerDialogCommand();

 signals:
  void StatusChanged();
  void WlanChanged();
  void EthernetChanged();
  void HotspotChanged();
  void AirplaneModeChanged();

  // Emitted when a secured AP has no saved connection and no password was
  // provided. The frontend should show a credential form for the given
  // security_category (NC_SEC_WEP, NC_SEC_WPA_PERSONAL, NC_SEC_ENTERPRISE)
  // and call ConnectWlan() or ConnectWlanAdvanced() with the result.
  void PasswordRequired(const QString& ssid, const QString& ap_path,
      const QString& security_category);

 private:
  void Initialize();
  void RefreshDevices();
  void RefreshWlanData();
  void RefreshEthernetData();
  void RefreshConnectivity();
  void RefreshHotspot();

  void SubscribeToDeviceProperties(const QString& device_path);

  // Returns the D-Bus object path of an existing NM connection profile whose
  // 802-11-wireless ssid matches ssid, or an empty string if not found.
  QString FindSavedConnection(const QByteArray& ssid) const;

  // Builds an NM connection settings map for a new WPA-PSK connection.
  static QVariantMap BuildWpaPskConnectionSettings(
      const QByteArray& ssid, const QString& password);

  // Builds an NM connection settings map for a WEP connection.
  static QVariantMap BuildWepConnectionSettings(const QByteArray& ssid,
      const QVariantMap& credentials);

  // Builds an NM connection settings map for a WPA-Enterprise (EAP) connection.
  static QVariantMap BuildEapConnectionSettings(const QByteArray& ssid,
      const QVariantMap& credentials);

  // Builds an NM connection settings map for a new open connection.
  static QVariantMap BuildOpenConnectionSettings(const QByteArray& ssid);

  // Returns a security category string (NC_SEC_*) derived from AP flag values.
  static QString SecurityCategoryFromFlags(
      uint ap_flags, uint wpa_flags, uint rsn_flags);

  // Encodes a file-system path as NM certificate byte-array ("file://path\0").
  static QByteArray PathToNmCertBytes(const QString& path);

  // Derives a human-readable security string from NM AP flag values.
  static QString SecurityTypeFromFlags(
      uint ap_flags, uint wpa_flags, uint rsn_flags);

  // Maps signal strength [0,100] to a Material Symbol icon name.
  static QString WlanIconFromStrength(int strength);

  // Reads the Strength property of an AP D-Bus object.
  static int GetApStrength(const QString& ap_path);

  // Converts a raw NM SSID byte array to a printable QString.
  static QString SsidToString(const QByteArray& ssid);

  // Reads a single D-Bus property from a NetworkManager object.
  static QVariant GetNmProperty(const QString& path,
      const QString& nm_interface,
      const QString& prop);

 private slots:
  void OnNmPropertiesChanged(const QString& nm_interface,
      const QVariantMap& changed,
      const QStringList& invalidated);
  void OnDevicePropertiesChanged(const QString& nm_interface,
      const QVariantMap& changed,
      const QStringList& invalidated);
  void OnNmStateChanged(uint state);
  void OnDeviceAdded(const QDBusObjectPath& device_path);
  void OnDeviceRemoved(const QDBusObjectPath& device_path);

 private:
  QDBusConnection bus_;

  bool has_wlan_{false};
  bool has_ethernet_{false};
  bool login_required_{false};
  bool airplane_mode_{false};

  QString selected_wlan_device_path_;

  QVariantList wlan_devices_;
  QVariantList ethernet_devices_;
  QVariantMap current_wlan_network_;
  QVariantList available_wlan_networks_;

  bool hotspot_enabled_{false};
  bool hotspot_available_{false};
  QString hotspot_name_;
  QString hotspot_password_;

  QStringList monitored_device_paths_;

  // path of AP that a ConnectWlan() call is in flight for (if any)
  QString pending_connect_ap_path_;
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_NETWORK_CONTROL_NETWORK_CONTROL_H_
