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

#include <QDBusInterface>
#include <QDebug>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/battery_info/battery_info.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

BatteryInfo::BatteryInfo() {
  QDBusConnection::systemBus().connect(
    DBUS_UPOWER_SERVICE, DBUS_UPOWER_DISPLAY_SERVICE_PATH,
    DBUS_PROPERTIES_INTERFACE, DBUS_PROP_UPDTE_METHOD, this,
    SLOT(OnUpdateProperties(QString, QVariantMap, QStringList)));
}


int BatteryInfo::GetBatteryLevel() {
  QDBusInterface interface(DBUS_UPOWER_SERVICE,
    DBUS_UPOWER_DISPLAY_SERVICE_PATH, DBUS_UPOWER_DEVICE_INTERFACE,
    QDBusConnection::systemBus());

  if (!interface.isValid()) {
    LOG(ERROR) << absl::StrCat("Failed to connect to UPower D-Bus interface.");
  }

  QVariant percentage = interface.property("Percentage");
  return percentage.toInt();
}

bool BatteryInfo::GetIsCharging() {
  QDBusInterface interface(DBUS_UPOWER_SERVICE,
    DBUS_UPOWER_DISPLAY_SERVICE_PATH, DBUS_UPOWER_DEVICE_INTERFACE,
    QDBusConnection::systemBus());

  if (!interface.isValid()) {
    LOG(ERROR) << absl::StrCat("Failed to connect to UPower D-Bus interface.");
  }

  QVariant state_raw = interface.property("State");
  int state = state_raw.toInt();
  return (state == 1 || state == 5);
}

void BatteryInfo::OnUpdateProperties(const QString& interface,
    const QVariantMap& changed, const QStringList& invalidated) {
  Q_UNUSED(changed);
  Q_UNUSED(invalidated);

  if (interface != DBUS_UPOWER_DEVICE_INTERFACE) {
    return;
  }

  emit StatusChanged();
}

}  // namespace backend
}  // namespace panel
