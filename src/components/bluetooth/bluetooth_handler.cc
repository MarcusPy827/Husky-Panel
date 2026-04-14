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

#include "src/components/bluetooth/bluetooth_handler.h"

#include <QProcess>
#include <QStringList>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"

namespace panel {
namespace frontend {

/**
 * @brief The constructor for @c BluetoothHandler.
 *
 * @details Creates the underlying @c backend::BluetoothInfo instance and
 *          wires its signals to the corresponding signals of this handler.
 * @param parent (QObject*) The parent QObject.
 * @return void.
 */

BluetoothHandler::BluetoothHandler(QObject* parent) : QObject(parent) {
  bluetooth_info_ = new backend::BluetoothInfo(this);

  connect(bluetooth_info_, &backend::BluetoothInfo::EnabledChanged,
    this, &BluetoothHandler::EnabledChanged);
  connect(bluetooth_info_, &backend::BluetoothInfo::AdaptersChanged,
    this, &BluetoothHandler::AdaptersChanged);
  connect(bluetooth_info_, &backend::BluetoothInfo::DevicesChanged,
    this, &BluetoothHandler::DevicesChanged);
}


/**
 * @brief Returns whether the selected Bluetooth adapter is powered on.
 *
 * @see @c bluetooth_info.h for @c BluetoothInfo::GetEnabled().
 * @return (bool) @c true if Bluetooth is enabled, @c false otherwise.
 */

bool BluetoothHandler::GetBluetoothEnabled() const {
  return bluetooth_info_->GetEnabled();
}


/**
 * @brief Returns the list of available Bluetooth adapters.
 *
 * @see @c bluetooth_info.h for @c BluetoothInfo::GetAdapters().
 * @return (QVariantList) A list of adapter QVariantMaps.
 */

QVariantList BluetoothHandler::GetAdapters() const {
  return bluetooth_info_->GetAdapters();
}


/**
 * @brief Returns the D-Bus object path of the currently selected adapter.
 *
 * @see @c bluetooth_info.h for @c BluetoothInfo::GetSelectedAdapterPath().
 * @return (QString) The selected adapter's object path.
 */

QString BluetoothHandler::GetSelectedAdapterPath() const {
  return bluetooth_info_->GetSelectedAdapterPath();
}


/**
 * @brief Returns the list of known Bluetooth devices for the selected
 *        adapter.
 *
 * @see @c bluetooth_info.h for @c BluetoothInfo::GetDevices().
 * @return (QVariantList) A list of device QVariantMaps.
 */

QVariantList BluetoothHandler::GetDevices() const {
  return bluetooth_info_->GetDevices();
}


/**
 * @brief Returns the Material Symbols icon name representing the current
 *        Bluetooth state.
 *
 * @note Icon mapping:
 *         Bluetooth disabled → @c bluetooth_disabled
 *         Enabled, no connected device → @c bluetooth
 *         Enabled, at least one connected device → @c bluetooth_connected
 * @return (QString) One of: @c bluetooth_disabled, @c bluetooth,
 *         @c bluetooth_connected.
 */

QString BluetoothHandler::GetBtIcon() const {
  if (!bluetooth_info_->GetEnabled())
    return QStringLiteral("bluetooth_disabled");

  for (const QVariant& v : bluetooth_info_->GetDevices()) {
    const QVariantMap m = v.toMap();
    if (m[QStringLiteral("state")].toString() ==
        QStringLiteral("connected"))
      return QStringLiteral("bluetooth_connected");
  }
  return QStringLiteral("bluetooth");
}


/**
 * @brief Powers the selected Bluetooth adapter on or off.
 *
 * @param enabled (bool) @c true to power on, @c false to power off.
 * @return void.
 */

void BluetoothHandler::setEnabled(bool enabled) {
  bluetooth_info_->SetEnabled(enabled);
}


/**
 * @brief Selects a Bluetooth adapter by its D-Bus object path.
 *
 * @param path (const QString&) The BlueZ D-Bus object path of the adapter
 *        to select.
 * @return void.
 */

void BluetoothHandler::selectAdapter(const QString& path) {
  bluetooth_info_->SelectAdapter(path);
}


/**
 * @brief Initiates an asynchronous connection to a Bluetooth device.
 *
 * @param address (const QString&) The Bluetooth address of the device.
 * @return void.
 */

void BluetoothHandler::connectDevice(const QString& address) {
  bluetooth_info_->ConnectDevice(address);
}


/**
 * @brief Disconnects from a Bluetooth device.
 *
 * @param address (const QString&) The Bluetooth address of the device.
 * @return void.
 */

void BluetoothHandler::disconnectDevice(const QString& address) {
  bluetooth_info_->DisconnectDevice(address);
}


/**
 * @brief Opens the system Bluetooth pairing settings dialog.
 *
 * @details Tries common Bluetooth management tools in order: the KDE
 *          Plasma control module, the GNOME control centre panel,
 *          Bluedevil's pairing wizard, and Blueman manager. The first
 *          command found in the system @c PATH is launched detached.
 * @return void.
 */

void BluetoothHandler::openPairingSettings() {
  struct Candidate {
    const char* program;
    QStringList args;
  };

  static const Candidate kCandidates[] = {
    {"kcmshell6", {QStringLiteral("kcm_bluetooth")}},
    {"bluedevil-wizard", {}},
    {"gnome-control-center", { QStringLiteral("bluetooth")}},
    {"blueman-manager", {}},
  };

  for (const auto& c : kCandidates) {
    if (QProcess::startDetached(
        QString::fromLatin1(c.program), c.args)) {
      LOG(INFO) << absl::StrCat(
        "BluetoothHandler: opened pairing settings via ", c.program);
      return;
    }
  }

  LOG(WARNING) << absl::StrCat(
    "BluetoothHandler: no Bluetooth settings tool found.");
}

}  // namespace frontend
}  // namespace panel
