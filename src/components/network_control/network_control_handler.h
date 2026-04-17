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

#ifndef SRC_COMPONENTS_NETWORK_CONTROL_NETWORK_CONTROL_HANDLER_H_
#define SRC_COMPONENTS_NETWORK_CONTROL_NETWORK_CONTROL_HANDLER_H_

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

#include "src/info_server/network_control/network_control.h"

namespace panel {
namespace frontend {

/**
 * @brief QML-facing bridge for network control.
 *
 * @details Injected into the QML engine as @c "NetworkHandler". All
 *          properties are read-only from QML; mutations are performed via
 *          @c Q_INVOKABLE methods.
 */

class NetworkControlHandler : public QObject {
  Q_OBJECT

  // ---- Status bar icon state ----
  Q_PROPERTY(bool showWlanIcon
    READ GetShowWlanIcon
    NOTIFY StatusChanged)

  Q_PROPERTY(bool showEthernetIcon
    READ GetShowEthernetIcon
    NOTIFY StatusChanged)

  Q_PROPERTY(bool showNoHardwareIcon
    READ GetShowNoHardwareIcon
    NOTIFY StatusChanged)

  Q_PROPERTY(bool loginRequired
    READ GetLoginRequired
    NOTIFY StatusChanged)

  // "settings_ethernet" | "globe_2_cancel"
  Q_PROPERTY(QString ethernetIcon
    READ GetEthernetIcon
    NOTIFY StatusChanged)

  // Material Symbol name for the active WiFi connection strength
  Q_PROPERTY(QString wlanIcon
    READ GetWlanIcon
    NOTIFY StatusChanged)

  // ---- General ----
  Q_PROPERTY(bool airplaneModeEnabled
    READ GetAirplaneModeEnabled
    NOTIFY AirplaneModeChanged)

  // ---- Wireless ----
  // [{name, path, enabled}]
  Q_PROPERTY(QVariantList wlanDevices
    READ GetWlanDevices
    NOTIFY WlanChanged)

  Q_PROPERTY(QString selectedWlanDevice
    READ GetSelectedWlanDevice
    NOTIFY WlanChanged)

  Q_PROPERTY(bool wlanDeviceEnabled
    READ GetWlanDeviceEnabled
    NOTIFY WlanChanged)

  // {ssid, strength, security, icon, status, isHotspot, isUsb, isBluetooth,
  //  apPath, devicePath}
  Q_PROPERTY(QVariantMap currentWlanNetwork
    READ GetCurrentWlanNetwork
    NOTIFY WlanChanged)

  // [{ssid, strength, security, apPath, icon, status,
  //   isHotspot, isUsb, isBluetooth}]
  Q_PROPERTY(QVariantList availableWlanNetworks
    READ GetAvailableWlanNetworks
    NOTIFY WlanChanged)

  // ---- Ethernet ----
  // [{name, path, status, icon, isHotspot, isUsb, isConnected}]
  Q_PROPERTY(QVariantList ethernetDevices
    READ GetEthernetDevices
    NOTIFY EthernetChanged)

  // ---- Hotspot ----
  Q_PROPERTY(bool hotspotEnabled
    READ GetHotspotEnabled
    NOTIFY HotspotChanged)

  Q_PROPERTY(bool hotspotAvailable
    READ GetHotspotAvailable
    NOTIFY HotspotChanged)

  Q_PROPERTY(QString hotspotName
    READ GetHotspotName
    NOTIFY HotspotChanged)

  Q_PROPERTY(QString hotspotPassword
    READ GetHotspotPassword
    NOTIFY HotspotChanged)

 public:
  /* ---------- For backend usage ---------- */
  explicit NetworkControlHandler(QObject* parent = nullptr);

  /**
   * @brief Returns @c true when at least one Wi-Fi device is present.
   * @return bool.
   */
  bool GetShowWlanIcon() const;

  /**
   * @brief Returns @c true when at least one Ethernet device is present.
   * @return bool.
   */
  bool GetShowEthernetIcon() const;

  /**
   * @brief Returns @c true when neither Wi-Fi nor Ethernet hardware is present.
   * @return bool.
   */
  bool GetShowNoHardwareIcon() const;

  /**
   * @brief Returns @c true when captive-portal connectivity is detected.
   * @return bool.
   */
  bool GetLoginRequired() const;

  /**
   * @brief Returns the Material Symbol icon name for the Ethernet status.
   * @return QString.
   */
  QString GetEthernetIcon() const;

  /**
   * @brief Returns the Material Symbol icon name for the active Wi-Fi signal.
   * @return QString.
   */
  QString GetWlanIcon() const;

  /**
   * @brief Returns @c true when the wireless radio is disabled.
   * @return bool.
   */
  bool GetAirplaneModeEnabled() const;

  /**
   * @brief Returns the list of known Wi-Fi devices.
   * @return QVariantList.
   */
  QVariantList GetWlanDevices() const;

  /**
   * @brief Returns the D-Bus object path of the selected Wi-Fi device.
   * @return QString.
   */
  QString GetSelectedWlanDevice() const;

  /**
   * @brief Returns @c true when the selected Wi-Fi device is active.
   * @return bool.
   */
  bool GetWlanDeviceEnabled() const;

  /**
   * @brief Returns the currently active Wi-Fi network properties.
   * @return QVariantMap.
   */
  QVariantMap GetCurrentWlanNetwork() const;

  /**
   * @brief Returns visible Wi-Fi access points excluding the active one.
   * @return QVariantList.
   */
  QVariantList GetAvailableWlanNetworks() const;

  /**
   * @brief Returns the list of known Ethernet devices.
   * @return QVariantList.
   */
  QVariantList GetEthernetDevices() const;

  /**
   * @brief Returns @c true when the hotspot is currently active.
   * @return bool.
   */
  bool GetHotspotEnabled() const;

  /**
   * @brief Returns @c true when hotspot creation is supported.
   * @return bool.
   */
  bool GetHotspotAvailable() const;

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

  /* ---------- For QML usage ---------- */

  /**
   * @brief Enables or disables the wireless radio (airplane mode).
   *
   * @param enabled (bool) Pass @c true to enable airplane mode.
   * @return void.
   */
  Q_INVOKABLE void setAirplaneMode(bool enabled);

  /**
   * @brief Switches the active Wi-Fi device to @p device_path.
   *
   * @param device_path (const QString&) D-Bus object path of the device.
   * @return void.
   */
  Q_INVOKABLE void selectWlanDevice(const QString& device_path);

  /**
   * @brief Enables or disables the selected Wi-Fi device.
   *
   * @param enabled (bool) Pass @c false to disconnect the device.
   * @return void.
   */
  Q_INVOKABLE void setWlanDeviceEnabled(bool enabled);

  /**
   * @brief Connects to a Wi-Fi access point.
   *
   * @details For secured networks with no saved profile, emits
   *          @c passwordRequired() when @p password is empty.
   * @param ap_path (const QString&) D-Bus object path of the access point.
   * @param password (const QString&) WPA pre-shared key, or empty string.
   * @return void.
   */
  Q_INVOKABLE void connectWlan(const QString& ap_path,
      const QString& password);

  /**
   * @brief Connects using explicit credentials for WEP or 802.1X networks.
   *
   * @details The @p credentials map must contain @c "type" set to
   *          @c "wep" or @c "enterprise" plus method-specific keys.
   * @param ap_path (const QString&) D-Bus object path of the access point.
   * @param credentials (const QVariantMap&) Credential map.
   * @return void.
   */
  Q_INVOKABLE void connectWlanAdvanced(const QString& ap_path,
      const QVariantMap& credentials);

  /**
   * @brief Disconnects the Wi-Fi device at @p device_path.
   *
   * @param device_path (const QString&) D-Bus object path of the device.
   * @return void.
   */
  Q_INVOKABLE void disconnectWlan(const QString& device_path);

  /**
   * @brief Disconnects the Ethernet device at @p device_path.
   *
   * @param device_path (const QString&) D-Bus object path of the device.
   * @return void.
   */
  Q_INVOKABLE void disconnectEthernet(const QString& device_path);

  /**
   * @brief Enables or disables the hotspot on a secondary Wi-Fi device.
   *
   * @param enabled (bool) Pass @c true to activate the hotspot.
   * @return void.
   */
  Q_INVOKABLE void setHotspotEnabled(bool enabled);

  /**
   * @brief Stores the hotspot SSID and passphrase for the next activation.
   *
   * @param name (const QString&) Desired hotspot SSID.
   * @param password (const QString&) Desired WPA passphrase.
   * @return void.
   */
  Q_INVOKABLE void setHotspotConfig(const QString& name,
      const QString& password);

  /**
   * @brief Launches the system network manager dialog.
   *
   * @details Used as the fallback for password entry. Tries @c plasma-nm,
   *          @c nm-connection-editor, or @c kcmshell6 in that order.
   * @return void.
   */
  Q_INVOKABLE void openSystemNetworkManager();

 signals:
  void StatusChanged();
  void WlanChanged();
  void EthernetChanged();
  void HotspotChanged();
  void AirplaneModeChanged();

  // Re-emitted from the backend. QML should show a credential form matching
  // security_category (NC_SEC_WEP / NC_SEC_WPA_PERSONAL / NC_SEC_ENTERPRISE)
  // then call connectWlan() or connectWlanAdvanced() with the result.
  void passwordRequired(const QString& ssid, const QString& ap_path,
      const QString& security_category);

 private:
  backend::NetworkControl* network_control_ = nullptr;
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_NETWORK_CONTROL_NETWORK_CONTROL_HANDLER_H_
