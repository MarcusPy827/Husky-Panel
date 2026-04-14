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

#ifndef SRC_INFO_SERVER_BLUETOOTH_BLUETOOTH_INFO_H_
#define SRC_INFO_SERVER_BLUETOOTH_BLUETOOTH_INFO_H_

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusPendingCallWatcher>
#include <QMap>
#include <QMetaType>
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

// D-Bus type aliases for the BlueZ ObjectManager interface.
// InterfaceMap = a{sa{sv}}  (interface name → property map)
// ManagedObjectMap = a{oa{sa{sv}}}  (object path → InterfaceMap)
using BlueZInterfaceMap = QMap<QString, QVariantMap>;
using BlueZObjectMap = QMap<QDBusObjectPath, BlueZInterfaceMap>;

Q_DECLARE_METATYPE(BlueZInterfaceMap)
Q_DECLARE_METATYPE(BlueZObjectMap)

namespace panel {
namespace backend {

// Monitors and controls Bluetooth via the BlueZ D-Bus API
// (org.bluez). Exposes adapter and device lists, power state, and
// connect/disconnect operations. All data is updated reactively via
// D-Bus signals and marshalled to the Qt main thread.
class BluetoothInfo : public QObject {
  Q_OBJECT

 public:
  // Possible fine-grained states for a device entry exposed to QML.
  enum class DeviceState {
    kConnected,
    kPaired,
    kConnecting,
    kFailed,
  };

  explicit BluetoothInfo(QObject* parent = nullptr);
  ~BluetoothInfo() override = default;

  // Returns a list of available Bluetooth adapters. Each entry is a
  // QVariantMap with keys: "address" (QString), "name" (QString),
  // "path" (QString).
  QVariantList GetAdapters() const;

  // Returns the D-Bus object path of the currently selected adapter.
  QString GetSelectedAdapterPath() const;

  // Returns whether the selected adapter is powered on.
  bool GetEnabled() const;

  // Returns the list of known devices for the selected adapter. Each
  // entry is a QVariantMap with keys: "address" (QString), "name"
  // (QString), "icon" (QString — material icon name), "state" (QString
  // — "connected" | "paired" | "connecting" | "failed").
  QVariantList GetDevices() const;

  // Powers the selected adapter on or off.
  void SetEnabled(bool enabled);

  // Changes the active adapter. @p path is a BlueZ D-Bus object path.
  void SelectAdapter(const QString& path);

  // Initiates an async D-Bus Connect() call on a device. The device's
  // state transitions to "connecting" immediately, and then to
  // "connected" or "failed" when the call completes.
  void ConnectDevice(const QString& address);

  // Initiates an async D-Bus Disconnect() call on a device.
  void DisconnectDevice(const QString& address);

 signals:
  void AdaptersChanged();
  void EnabledChanged();
  void DevicesChanged();

 private:
  // Fetches all managed BlueZ objects and rebuilds adapters/devices.
  void RefreshAll();

  // Rebuilds adapters_ from managed_objects_.
  void RebuildAdapters();

  // Rebuilds devices_ for the selected adapter from managed_objects_.
  void RebuildDevices();

  // Returns the material icon name for a BlueZ device icon string.
  static QString IconForDevice(const QString& bluez_icon);

  // Returns the string key for a DeviceState.
  static QString StateString(DeviceState s);

 private slots:
  // Slot: receives org.freedesktop.DBus.ObjectManager.InterfacesAdded.
  void OnInterfacesAdded(const QDBusObjectPath& path,
    const BlueZInterfaceMap& ifaces);

  // Slot: receives org.freedesktop.DBus.ObjectManager.InterfacesRemoved.
  void OnInterfacesRemoved(const QDBusObjectPath& path,
    const QStringList& ifaces);

  // Slot: receives org.freedesktop.DBus.Properties.PropertiesChanged from
  // any BlueZ object (wildcard path match). Triggers a full refresh of all
  // managed objects so that adapter/device data stays consistent.
  void OnPropertiesChanged(const QString& iface,
    const QVariantMap& changed, const QStringList& invalidated);

 private:
  QDBusConnection bus_ = QDBusConnection::systemBus();

  // All objects returned by GetManagedObjects, keyed by object path string.
  // Stored as QMap<QString, BlueZInterfaceMap> for easy lookup by path string.
  QMap<QString, BlueZInterfaceMap> managed_objects_;

  QString selected_adapter_path_;
  QVariantList adapters_;
  QVariantList devices_;

  // Per-device overrides for connecting/failed states (address → state).
  QMap<QString, DeviceState> device_state_overrides_;
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_BLUETOOTH_BLUETOOTH_INFO_H_
