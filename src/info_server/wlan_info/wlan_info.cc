/*
 * Copyright (C) 2025 MarcusPy827
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

#include <QDBusInterface>
#include <QDBusReply>
#include <QList>
#include <QDebug>

#include "src/info_server/wlan_info/wlan_info.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

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

    // Check if current device is a Wi-Fi device
    if (type_var.isValid() && type_var.toUInt() == WI_WIFI_TYPE) {
      QDBusInterface wlan_interface(DBUS_NETWORK_MANAGER_SERVICE,
        device_path.path(), DBUS_NETWORK_MANAGER_WIRELESS_INTERFACE,
        QDBusConnection::systemBus());

      QVariant ap_path_raw = wlan_interface.property("ActiveAccessPoint");
      QDBusObjectPath ap_path = ap_path_raw.value<QDBusObjectPath>();

      // Case 1: WLAN disconnected
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
