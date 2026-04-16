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

#include "src/components/network_control/network_control_handler.h"

#include <QProcess>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"

namespace panel {
namespace frontend {

NetworkControlHandler::NetworkControlHandler(QObject* parent)
    : QObject(parent) {
  LOG(INFO) << absl::StrCat("Initializing network control handler...");
  network_control_ = new backend::NetworkControl(this);

  connect(network_control_, &backend::NetworkControl::StatusChanged,
    this, &NetworkControlHandler::StatusChanged);
  connect(network_control_, &backend::NetworkControl::WlanChanged,
    this, &NetworkControlHandler::WlanChanged);
  connect(network_control_, &backend::NetworkControl::EthernetChanged,
    this, &NetworkControlHandler::EthernetChanged);
  connect(network_control_, &backend::NetworkControl::HotspotChanged,
    this, &NetworkControlHandler::HotspotChanged);
  connect(network_control_, &backend::NetworkControl::AirplaneModeChanged,
    this, &NetworkControlHandler::AirplaneModeChanged);
  connect(network_control_, &backend::NetworkControl::PasswordRequired,
    this, &NetworkControlHandler::passwordRequired);
}

// ---------------------------------------------------------------------------
// Property getters
// ---------------------------------------------------------------------------

bool NetworkControlHandler::GetShowWlanIcon() const {
  return network_control_->HasWlanHardware();
}

bool NetworkControlHandler::GetShowEthernetIcon() const {
  return network_control_->HasEthernetHardware();
}

bool NetworkControlHandler::GetShowNoHardwareIcon() const {
  return !network_control_->HasWlanHardware()
      && !network_control_->HasEthernetHardware();
}

bool NetworkControlHandler::GetLoginRequired() const {
  return network_control_->IsLoginRequired();
}

QString NetworkControlHandler::GetEthernetIcon() const {
  return network_control_->GetEthernetIcon();
}

QString NetworkControlHandler::GetWlanIcon() const {
  return network_control_->GetWlanIcon();
}

bool NetworkControlHandler::GetAirplaneModeEnabled() const {
  return network_control_->IsAirplaneModeEnabled();
}

QVariantList NetworkControlHandler::GetWlanDevices() const {
  return network_control_->GetWlanDevices();
}

QString NetworkControlHandler::GetSelectedWlanDevice() const {
  return network_control_->GetSelectedWlanDevice();
}

bool NetworkControlHandler::GetWlanDeviceEnabled() const {
  return network_control_->IsSelectedWlanDeviceEnabled();
}

QVariantMap NetworkControlHandler::GetCurrentWlanNetwork() const {
  return network_control_->GetCurrentWlanNetwork();
}

QVariantList NetworkControlHandler::GetAvailableWlanNetworks() const {
  return network_control_->GetAvailableWlanNetworks();
}

QVariantList NetworkControlHandler::GetEthernetDevices() const {
  return network_control_->GetEthernetDevices();
}

bool NetworkControlHandler::GetHotspotEnabled() const {
  return network_control_->IsHotspotEnabled();
}

bool NetworkControlHandler::GetHotspotAvailable() const {
  return network_control_->IsHotspotAvailable();
}

QString NetworkControlHandler::GetHotspotName() const {
  return network_control_->GetHotspotName();
}

QString NetworkControlHandler::GetHotspotPassword() const {
  return network_control_->GetHotspotPassword();
}

// ---------------------------------------------------------------------------
// Invokables
// ---------------------------------------------------------------------------

void NetworkControlHandler::setAirplaneMode(bool enabled) {
  network_control_->SetAirplaneMode(enabled);
}

void NetworkControlHandler::selectWlanDevice(const QString& device_path) {
  network_control_->SelectWlanDevice(device_path);
}

void NetworkControlHandler::setWlanDeviceEnabled(bool enabled) {
  network_control_->SetWlanDeviceEnabled(enabled);
}

void NetworkControlHandler::connectWlan(const QString& ap_path,
    const QString& password) {
  network_control_->ConnectWlan(ap_path, password);
}

void NetworkControlHandler::disconnectWlan(const QString& device_path) {
  network_control_->DisconnectWlan(device_path);
}

void NetworkControlHandler::disconnectEthernet(const QString& device_path) {
  network_control_->DisconnectEthernet(device_path);
}

void NetworkControlHandler::setHotspotEnabled(bool enabled) {
  network_control_->SetHotspotEnabled(enabled);
}

void NetworkControlHandler::setHotspotConfig(const QString& name,
    const QString& password) {
  network_control_->SetHotspotConfig(name, password);
}

void NetworkControlHandler::openSystemNetworkManager() {
  QString cmd = backend::NetworkControl::GetNetworkManagerDialogCommand();
  LOG(INFO) << absl::StrCat("Launching network manager dialog: ",
    cmd.toStdString());
  QProcess::startDetached(
    QStringLiteral("/bin/sh"),
    {QStringLiteral("-c"), cmd});
}

}  // namespace frontend
}  // namespace panel
