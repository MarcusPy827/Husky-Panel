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

#include "src/info_server/bluetooth/bluetooth_info.h"

#include <QDBusArgument>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusReply>

#include "absl/log/log.h"
#include "src/utils/dbus_def.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

namespace panel {
namespace backend {

/**
 * @brief The constructor for @c BluetoothInfo.
 *
 * @details Connects to the BlueZ D-Bus service on the system bus,
 *          subscribes to @c InterfacesAdded, @c InterfacesRemoved, and
 *          @c PropertiesChanged signals, and performs an initial refresh
 *          of all managed objects.
 * @param parent (QObject*) The parent QObject.
 * @return void.
 */

BluetoothInfo::BluetoothInfo(QObject* parent) : QObject(parent) {
  // Register D-Bus types so Qt can marshal/unmarshal them automatically.
  qDBusRegisterMetaType<BlueZInterfaceMap>();
  qDBusRegisterMetaType<BlueZObjectMap>();

  // Subscribe to ObjectManager signals for adapter/device hotplug.
  bus_.connect(
    DBUS_BLUEZ_SERVICE, DBUS_BLUEZ_ROOT_PATH, DBUS_OBJECT_MANAGER_INTERFACE,
    QStringLiteral("InterfacesAdded"),
    this,
    SLOT(OnInterfacesAdded(QDBusObjectPath,
      QMap<QString, QVariantMap>)));

  bus_.connect(
    DBUS_BLUEZ_SERVICE, DBUS_BLUEZ_ROOT_PATH, DBUS_OBJECT_MANAGER_INTERFACE,
    QStringLiteral("InterfacesRemoved"),
    this,
    SLOT(OnInterfacesRemoved(QDBusObjectPath, QStringList)));

  // Subscribe to property changes on all BlueZ objects (wildcard path).
  bus_.connect(
    DBUS_BLUEZ_SERVICE, QString(), DBUS_PROPERTIES_INTERFACE,
    QStringLiteral("PropertiesChanged"),
    this,
    SLOT(OnPropertiesChanged(QString, QVariantMap, QStringList)));

  RefreshAll();
}


/* ---------- Public getters ---------- */


/**
 * @brief Returns the list of available Bluetooth adapters.
 *
 * @return (QVariantList) Each entry is a QVariantMap with keys
 *         @c address, @c name, and @c path.
 */

QVariantList BluetoothInfo::GetAdapters() const {
  return adapters_;
}


/**
 * @brief Returns the D-Bus object path of the selected adapter.
 *
 * @return (QString) The selected adapter's BlueZ object path, or empty
 *         string if no adapter is available.
 */

QString BluetoothInfo::GetSelectedAdapterPath() const {
  return selected_adapter_path_;
}


/**
 * @brief Returns whether the selected adapter is powered on.
 *
 * @return (bool) @c true if the selected adapter's @c Powered property is
 *         @c true, @c false otherwise or if no adapter is selected.
 */

bool BluetoothInfo::GetEnabled() const {
  if (selected_adapter_path_.isEmpty()) return false;

  auto it = managed_objects_.find(selected_adapter_path_);
  if (it == managed_objects_.end()) return false;

  auto iface_it = it->find(DBUS_BLUEZ_ADAPTER_INTERFACE);
  if (iface_it == it->end()) return false;

  return iface_it->value(QStringLiteral("Powered")).toBool();
}


/**
 * @brief Returns the list of known Bluetooth devices for the selected
 *        adapter.
 *
 * @return (QVariantList) Each entry is a QVariantMap with keys @c address,
 *         @c name, @c icon, and @c state.
 */

QVariantList BluetoothInfo::GetDevices() const {
  return devices_;
}


/* ---------- Public operations ---------- */


/**
 * @brief Powers the selected Bluetooth adapter on or off.
 *
 * @details Calls @c org.freedesktop.DBus.Properties.Set on the selected
 *          adapter with the @c Powered property.
 * @param enabled (bool) @c true to power on, @c false to power off.
 * @return void.
 */

void BluetoothInfo::SetEnabled(bool enabled) {
  if (selected_adapter_path_.isEmpty()) return;

  QDBusInterface props_iface(
    DBUS_BLUEZ_SERVICE, selected_adapter_path_,
    DBUS_PROPERTIES_INTERFACE, bus_);

  props_iface.asyncCall(
    QStringLiteral("Set"),
    QString(DBUS_BLUEZ_ADAPTER_INTERFACE),
    QStringLiteral("Powered"),
    QVariant::fromValue(QDBusVariant(enabled)));
}


/**
 * @brief Selects a Bluetooth adapter by its D-Bus object path.
 *
 * @details Switches the active adapter and rebuilds the device list.
 * @param path (const QString&) The BlueZ D-Bus object path of the adapter.
 * @return void.
 */

void BluetoothInfo::SelectAdapter(const QString& path) {
  if (selected_adapter_path_ == path) return;
  selected_adapter_path_ = path;
  device_state_overrides_.clear();
  RebuildDevices();
  emit EnabledChanged();
  emit DevicesChanged();
}


/**
 * @brief Initiates an asynchronous Bluetooth connection to a device.
 *
 * @details Sets the device's state to @c connecting immediately, emits
 *          @c DevicesChanged, then calls @c org.bluez.Device1.Connect.
 *          On completion, state transitions to @c connected or @c failed.
 * @param address (const QString&) The Bluetooth address of the device to
 *        connect.
 * @return void.
 */

void BluetoothInfo::ConnectDevice(const QString& address) {
  // Find the device path for this address.
  QString device_path;
  for (auto it = managed_objects_.begin();
      it != managed_objects_.end(); ++it) {
    auto dev_it = it->find(DBUS_BLUEZ_DEVICE_INTERFACE);
    if (dev_it == it->end()) continue;
    if (dev_it->value(QStringLiteral("Address")).toString() == address) {
      device_path = it.key();
      break;
    }
  }

  if (device_path.isEmpty()) {
    LOG(WARNING) << absl::StrFormat(
      "BluetoothInfo: ConnectDevice: no path for address \"%s\"",
      address.toStdString());
    return;
  }

  device_state_overrides_[address] = DeviceState::kConnecting;
  RebuildDevices();
  emit DevicesChanged();

  QDBusInterface dev_iface(
    DBUS_BLUEZ_SERVICE, device_path, DBUS_BLUEZ_DEVICE_INTERFACE, bus_);
  QDBusPendingCall pending = dev_iface.asyncCall(QStringLiteral("Connect"));

  auto* watcher = new QDBusPendingCallWatcher(pending, this);
  connect(watcher, &QDBusPendingCallWatcher::finished,
    this, [this, address, watcher]() {
      QDBusPendingReply<> reply = *watcher;
      if (reply.isError()) {
        LOG(WARNING) << absl::StrFormat(
          "BluetoothInfo: Connect() failed for \"%s\": %s",
          address.toStdString(),
          reply.error().message().toStdString());
        device_state_overrides_[address] = DeviceState::kFailed;
      } else {
        device_state_overrides_.remove(address);
      }
      RebuildDevices();
      emit DevicesChanged();
      watcher->deleteLater();
    });
}


/**
 * @brief Initiates an asynchronous Bluetooth disconnection from a device.
 *
 * @details Calls @c org.bluez.Device1.Disconnect asynchronously.
 * @param address (const QString&) The Bluetooth address of the device to
 *        disconnect.
 * @return void.
 */

void BluetoothInfo::DisconnectDevice(const QString& address) {
  QString device_path;
  for (auto it = managed_objects_.begin();
      it != managed_objects_.end(); ++it) {
    auto dev_it = it->find(DBUS_BLUEZ_DEVICE_INTERFACE);
    if (dev_it == it->end()) continue;
    if (dev_it->value(QStringLiteral("Address")).toString() == address) {
      device_path = it.key();
      break;
    }
  }

  if (device_path.isEmpty()) return;

  device_state_overrides_.remove(address);

  QDBusInterface dev_iface(
    DBUS_BLUEZ_SERVICE, device_path, DBUS_BLUEZ_DEVICE_INTERFACE, bus_);
  dev_iface.asyncCall(QStringLiteral("Disconnect"));
}


/* ---------- Private helpers ---------- */


/**
 * @brief Fetches all objects from the BlueZ ObjectManager and rebuilds
 *        the adapter and device lists.
 *
 * @details Calls @c GetManagedObjects on @c org.bluez and caches the
 *          result in @c managed_objects_.
 * @return void.
 */

void BluetoothInfo::RefreshAll() {
  QDBusInterface obj_mgr(
    DBUS_BLUEZ_SERVICE, DBUS_BLUEZ_ROOT_PATH, DBUS_OBJECT_MANAGER_INTERFACE, bus_);

  if (!obj_mgr.isValid()) {
    LOG(WARNING) << absl::StrCat(
      "BluetoothInfo: BlueZ ObjectManager is not available.");
    return;
  }

  QDBusReply<BlueZObjectMap> reply =
    obj_mgr.call(QStringLiteral("GetManagedObjects"));
  if (!reply.isValid()) {
    LOG(WARNING) << absl::StrFormat(
      "BluetoothInfo: GetManagedObjects error: %s",
      reply.error().message().toStdString());
    return;
  }

  // Convert QDBusObjectPath keys to QString for easier lookup.
  managed_objects_.clear();
  const BlueZObjectMap& raw = reply.value();
  for (auto it = raw.begin(); it != raw.end(); ++it)
    managed_objects_[it.key().path()] = it.value();

  RebuildAdapters();
  RebuildDevices();

  emit AdaptersChanged();
  emit EnabledChanged();
  emit DevicesChanged();
}


/**
 * @brief Rebuilds the adapter list from @c managed_objects_.
 *
 * @details Selects the first adapter automatically if none is set.
 * @return void.
 */

void BluetoothInfo::RebuildAdapters() {
  adapters_.clear();

  for (auto it = managed_objects_.begin();
      it != managed_objects_.end(); ++it) {
    auto iface_it = it->find(DBUS_BLUEZ_ADAPTER_INTERFACE);
    if (iface_it == it->end()) continue;

    QVariantMap entry;
    entry[QStringLiteral("path")] = it.key();
    entry[QStringLiteral("address")] =
      iface_it->value(QStringLiteral("Address")).toString();
    entry[QStringLiteral("name")] =
      iface_it->value(QStringLiteral("Alias")).toString();
    adapters_.append(entry);
  }

  // Auto-select first adapter if none selected.
  if (selected_adapter_path_.isEmpty() && !adapters_.isEmpty()) {
    selected_adapter_path_ =
      adapters_.first().toMap()[QStringLiteral("path")].toString();
  }
  // NOTE: callers are responsible for emitting AdaptersChanged().
}


/**
 * @brief Rebuilds the device list for the selected adapter from
 *        @c managed_objects_.
 *
 * @return void.
 */

void BluetoothInfo::RebuildDevices() {
  devices_.clear();
  if (selected_adapter_path_.isEmpty()) return;

  // Devices whose path starts with selected_adapter_path_ + "/dev_"
  // belong to this adapter.
  QString prefix = selected_adapter_path_ + QStringLiteral("/dev_");

  for (auto it = managed_objects_.begin();
      it != managed_objects_.end(); ++it) {
    if (!it.key().startsWith(prefix)) continue;

    auto dev_it = it->find(DBUS_BLUEZ_DEVICE_INTERFACE);
    if (dev_it == it->end()) continue;

    const QVariantMap& props = *dev_it;
    bool connected = props.value(QStringLiteral("Connected")).toBool();
    bool paired = props.value(QStringLiteral("Paired")).toBool();

    // Only show paired or connected devices.
    if (!paired && !connected) continue;

    QString address = props.value(QStringLiteral("Address")).toString();
    QString name = props.value(QStringLiteral("Name")).toString();
    if (name.isEmpty()) name = address;

    QString bluez_icon = props.value(QStringLiteral("Icon")).toString();

    // Determine state — override takes priority.
    DeviceState state;
    auto ovr = device_state_overrides_.find(address);
    if (ovr != device_state_overrides_.end()) {
      state = ovr.value();
    } else if (connected) {
      state = DeviceState::kConnected;
    } else {
      state = DeviceState::kPaired;
    }

    QVariantMap entry;
    entry[QStringLiteral("address")] = address;
    entry[QStringLiteral("name")] = name;
    entry[QStringLiteral("icon")] = IconForDevice(bluez_icon);
    entry[QStringLiteral("state")] = StateString(state);
    devices_.append(entry);
  }
}


/**
 * @brief Maps a BlueZ device icon string to a Material Symbols name.
 *
 * @param bluez_icon (const QString&) The BlueZ @c Icon property value
 *        (e.g., @c "audio-headphones", @c "input-mouse").
 * @return (QString) One of: @c headphones, @c mouse, @c laptop_mac,
 *         @c desktop_mac, @c speaker, @c device_hub (fallback).
 */

QString BluetoothInfo::IconForDevice(const QString& bluez_icon) {
  if (bluez_icon.contains(QStringLiteral("headphone")) ||
      bluez_icon.contains(QStringLiteral("headset")))
    return QStringLiteral("headphones");
  if (bluez_icon.contains(QStringLiteral("mouse")))
    return QStringLiteral("mouse");
  if (bluez_icon.contains(QStringLiteral("laptop")))
    return QStringLiteral("laptop_mac");
  if (bluez_icon.contains(QStringLiteral("computer")))
    return QStringLiteral("desktop_mac");
  if (bluez_icon.contains(QStringLiteral("audio")) ||
      bluez_icon.contains(QStringLiteral("speaker")))
    return QStringLiteral("speaker");
  return QStringLiteral("device_hub");
}


/**
 * @brief Returns the QML-facing string for a @c DeviceState value.
 *
 * @param s (DeviceState) The state to convert.
 * @return (QString) One of: @c connected, @c paired, @c connecting,
 *         @c failed.
 */

QString BluetoothInfo::StateString(DeviceState s) {
  switch (s) {
    case DeviceState::kConnected:  return QStringLiteral("connected");
    case DeviceState::kPaired:     return QStringLiteral("paired");
    case DeviceState::kConnecting: return QStringLiteral("connecting");
    case DeviceState::kFailed:     return QStringLiteral("failed");
  }
  return QStringLiteral("paired");
}


/* ---------- D-Bus signal slots ---------- */


/**
 * @brief Handles @c InterfacesAdded from the BlueZ ObjectManager.
 *
 * @details Adds the new object to @c managed_objects_ and triggers a
 *          rebuild of adapters and/or devices as appropriate.
 * @param path (const QDBusObjectPath&) The object path added.
 * @param ifaces (const QMap<QString, QVariantMap>&) Interfaces and their
 *        properties.
 * @return void.
 */

void BluetoothInfo::OnInterfacesAdded(const QDBusObjectPath& path,
    const BlueZInterfaceMap& ifaces) {
  managed_objects_[path.path()] = ifaces;

  bool adapters_dirty = ifaces.contains(DBUS_BLUEZ_ADAPTER_INTERFACE);
  bool devices_dirty  = ifaces.contains(DBUS_BLUEZ_DEVICE_INTERFACE);

  if (adapters_dirty) {
    RebuildAdapters();
    emit AdaptersChanged();
    emit EnabledChanged();
  }
  if (devices_dirty) {
    RebuildDevices();
    emit DevicesChanged();
  }
}


/**
 * @brief Handles @c InterfacesRemoved from the BlueZ ObjectManager.
 *
 * @details Removes the relevant interfaces and triggers a rebuild.
 * @param path (const QDBusObjectPath&) The object path whose interfaces
 *        were removed.
 * @param ifaces (const QStringList&) The interface names removed.
 * @return void.
 */

void BluetoothInfo::OnInterfacesRemoved(const QDBusObjectPath& path,
    const QStringList& ifaces) {
  auto it = managed_objects_.find(path.path());
  if (it == managed_objects_.end()) return;

  bool adapters_dirty = ifaces.contains(DBUS_BLUEZ_ADAPTER_INTERFACE);
  bool devices_dirty  = ifaces.contains(DBUS_BLUEZ_DEVICE_INTERFACE);

  for (const QString& iface : ifaces)
    it->remove(iface);

  if (it->isEmpty())
    managed_objects_.erase(it);

  if (adapters_dirty) {
    if (selected_adapter_path_ == path.path())
      selected_adapter_path_.clear();
    RebuildAdapters();
    emit AdaptersChanged();
    emit EnabledChanged();
  }
  if (devices_dirty) {
    RebuildDevices();
    emit DevicesChanged();
  }
}


/**
 * @brief Handles @c PropertiesChanged from any BlueZ object.
 *
 * @details Because the subscription uses a wildcard object path, the
 *          sender's path is not available in the slot parameters. A full
 *          @c GetManagedObjects refresh is performed so that cached data
 *          stays consistent, and the appropriate signals are emitted based
 *          on which interface reported the change.
 * @param iface (const QString&) The interface whose properties changed.
 * @param changed (const QVariantMap&) Map of property name to new value.
 * @param invalidated (const QStringList&) Invalidated property names
 *        (unused — RefreshAll re-reads everything).
 * @return void.
 */

void BluetoothInfo::OnPropertiesChanged(const QString& /*iface*/,
    const QVariantMap& /*changed*/, const QStringList& /*invalidated*/) {
  // The wildcard-path subscription means we cannot identify which object
  // changed. A full refresh keeps managed_objects_ consistent; RefreshAll
  // emits all changed signals itself.
  RefreshAll();
}

}  // namespace backend
}  // namespace panel
