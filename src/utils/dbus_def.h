
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

// Battery Info
#define DBUS_UPOWER_SERVICE "org.freedesktop.UPower"
#define DBUS_UPOWER_DISPLAY_SERVICE_PATH \
  "/org/freedesktop/UPower/devices/DisplayDevice"
#define DBUS_UPOWER_DEVICE_INTERFACE "org.freedesktop.UPower.Device"
#define DBUS_PROPERTIES_INTERFACE "org.freedesktop.DBus.Properties"
#define DBUS_PROP_UPDTE_METHOD "PropertiesChanged"

// KDE KRunner
#define DBUS_KRUNNER_SERVICE "org.kde.krunner"
#define DBUS_KRUNNER_APP_PATH "/App"
#define DBUS_KRUNNER_APP_INTERFACE "org.kde.krunner.App"

// User Info
#define DBUS_ACCOUNTS_SERVICE "org.freedesktop.Accounts"
#define DBUS_ACCOUNTS_USER_INTERFACE "org.freedesktop.Accounts.User"

// WLAN
#define DBUS_NETWORK_MANAGER_SERVICE "org.freedesktop.NetworkManager"
#define DBUS_NETWORK_MANAGER_PATH "/org/freedesktop/NetworkManager"
#define DBUS_NETWORK_MANAGER_INTERFACE "org.freedesktop.NetworkManager"
#define DBUS_NETWORK_MANAGER_DEVICE_INTERFACE \
  "org.freedesktop.NetworkManager.Device"
#define DBUS_NETWORK_MANAGER_WIRELESS_INTERFACE \
  "org.freedesktop.NetworkManager.Device.Wireless"
#define DBUS_NETWORK_MANAGER_ACCESS_POINT_INTERFACE \
  "org.freedesktop.NetworkManager.AccessPoint"

// Current Window Info (KWin)
#define DBUS_KWIN_SERVICE "org.kde.KWin"
#define DBUS_CUSTOM_KWIN_WINDOW_SERVICE "marcus.panel.util.AppBridge"
#define DBUS_CUSTOM_KWIN_WINDOW_PATH "/AppBridge"
#define DBUS_CUSTOM_KWIN_WINDOW_INTERFACE "marcus.panel.util.AppBridge"
#define DBUS_CUSTOM_KWIN_WINDOW_INFO_INTERFACE "UpdateActiveApp"

// System Tray
#define DBUS_STATUS_NOTIFIER_WATCHER_SERVICE "org.kde.StatusNotifierWatcher"
#define DBUS_STATUS_NOTIFIER_WATCHER_PATH "/StatusNotifierWatcher"
#define DBUS_STATUS_NOTIFIER_WATCHER_INTERFACE "org.kde.StatusNotifierWatcher"
#define DBUS_STATUS_NOTIFIER_WATCHER_METHOD "RegisterStatusNotifierHost"
#define DBUS_STATUS_NOTIFIER_WATCHER_ICON_REGISTERED_METHOD \
  "StatusNotifierItemRegistered"
#define DBUS_STATUS_NOTIFIER_WATCHER_ICON_UNREGISTERED_METHOD \
  "StatusNotifierItemUnregistered"
#define DBUS_STATUS_NOTIFIER_WATCHER_GET_REGISTERED_METHOD \
  "RegisteredStatusNotifierItems"

// Session Handler
#define DBUS_LOGIND_SERVICE "org.freedesktop.login1"
#define DBUS_LOGIND_PATH "/org/freedesktop/login1"
#define DBUS_LOGIND_MANAGER_INTERFACE "org.freedesktop.login1.Manager"
#define DBUS_LOGIND_SESSION_INTERFACE "org.freedesktop.login1.Session"
#define DBUS_GNOME_SESSION_MANAGER_SERVICE "org.gnome.SessionManager"
#define DBUS_GNOME_SESSION_MANAGER_PATH "/org/gnome/SessionManager"
#define DBUS_GNOME_SESSION_MANAGER_INTERFACE "org.gnome.SessionManager"
#define DBUS_KDE_SHUTDOWN_SERVICE "org.kde.Shutdown"
#define DBUS_KDE_SHUTDOWN_PATH "/Shutdown"
#define DBUS_KDE_SHUTDOWN_INTERFACE "org.kde.Shutdown"
#define DBUS_KDE_SHUTDOWN_LOGOUT_METHOD "logout"
#define DBUS_XFCE_SESSION_MANAGER_SERVICE "org.xfce.SessionManager"
#define DBUS_XFCE_SESSION_MANAGER_PATH "/org/xfce/SessionManager"
#define DBUS_XFCE_SESSION_MANAGER_INTERFACE "org.xfce.Session.Manager"
#define DBUS_MATE_SESSION_MANAGER_SERVICE "org.mate.SessionManager"
#define DBUS_MATE_SESSION_MANAGER_PATH "/org/mate/SessionManager"
#define DBUS_MATE_SESSION_MANAGER_INTERFACE "org.mate.SessionManager"
#define DBUS_DISPLAY_MANAGER_SEAT_PATH "/org/freedesktop/DisplayManager"
#define DBUS_DISPLAY_MANAGER_SERVICE "org.freedesktop.DisplayManager"
#define DBUS_DISPLAY_MANAGER_SEAT_INTERFACE \
  "org.freedesktop.DisplayManager.Seat"

// Bluetooth (BlueZ)
#define DBUS_BLUEZ_SERVICE "org.bluez"
#define DBUS_BLUEZ_ROOT_PATH "/"
#define DBUS_OBJECT_MANAGER_INTERFACE "org.freedesktop.DBus.ObjectManager"
#define DBUS_BLUEZ_ADAPTER_INTERFACE "org.bluez.Adapter1"
#define DBUS_BLUEZ_DEVICE_INTERFACE "org.bluez.Device1"

#endif  // SRC_UTILS_DBUS_DEF_H_
