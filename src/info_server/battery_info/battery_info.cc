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
#include <QDebug>

#include "src/info_server/battery_info/battery_info.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

int BatteryInfo::GetBatteryLevel() {
  QDBusInterface interface(DBUS_UPOWER_SERVICE,
    DBUS_UPOWER_DISPLAY_SERVICE_PATH, DBUS_UPOWER_DEVICE_INTERFACE,
    QDBusConnection::systemBus());

  if (!interface.isValid()) {
    qCritical() << "[ERROR] Battery Info: Failed to connect to UPower D-Bus"
      << "interface.";
    return -1;
  }
  QVariant percentage = interface.property("Percentage");
  return percentage.toInt();
}

}  // namespace backend
}  // namespace panel
