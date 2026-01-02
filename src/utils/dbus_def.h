
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

#ifndef SRC_UTILS_DBUS_DEF_H_
#define SRC_UTILS_DBUS_DEF_H_

#define DBUS_UPOWER_SERVICE "org.freedesktop.UPower"
#define DBUS_UPOWER_DISPLAY_SERVICE_PATH \
  "/org/freedesktop/UPower/devices/DisplayDevice"
#define DBUS_UPOWER_DEVICE_INTERFACE "org.freedesktop.UPower.Device"

#define DBUS_KRUNNER_SERVICE "org.kde.krunner"
#define DBUS_KRUNNER_APP_PATH "/App"
#define DBUS_KRUNNER_APP_INTERFACE "org.kde.krunner.App"

#define DBUS_ACCOUNTS_SERVICE "org.freedesktop.Accounts"
#define DBUS_ACCOUNTS_USER_INTERFACE "org.freedesktop.Accounts.User"

#define DBUS_NETWORK_MANAGER_SERVICE "org.freedesktop.NetworkManager"
#define DBUS_NETWORK_MANAGER_PATH "/org/freedesktop/NetworkManager"
#define DBUS_NETWORK_MANAGER_INTERFACE "org.freedesktop.NetworkManager"
#define DBUS_NETWORK_MANAGER_DEVICE_INTERFACE \
  "org.freedesktop.NetworkManager.Device"
#define DBUS_NETWORK_MANAGER_WIRELESS_INTERFACE \
  "org.freedesktop.NetworkManager.Device.Wireless"
#define DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE \
  "org.freedesktop.NetworkManager.AccessPoint"

#endif  // SRC_UTILS_DBUS_DEF_H_
