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

#include <QDBusConnection>
#include <QDBusInterface>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/power_options/power_options.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

PowerOptions::PowerOptions(QObject* parent) : QObject(parent) {
  LOG(INFO) << absl::StrCat("PowerOptions constructer invoked!!");
}

// Note: The false here is for "interactive = false", which means that don't
// show a polkit dialog, just act.
void PowerOptions::CallLogind(const QString& method) {
  QDBusInterface target_interface(DBUS_LOGIND_SERVICE, DBUS_LOGIND_PATH,
    DBUS_LOGIND_MANAGER_INTERFACE, QDBusConnection::systemBus());
  target_interface.call(method, false);
}

void PowerOptions::Suspend() {
  CallLogind("Suspend");
}

void PowerOptions::Hibernate() {
  CallLogind("Hibernate");
}

void PowerOptions::Reboot() {
  CallLogind("Reboot");
}

void PowerOptions::PowerOff() {
  CallLogind("PowerOff");
}

void PowerOptions::suspend() {
  Suspend();
}

void PowerOptions::hibernate() {
  Hibernate();
}

void PowerOptions::reboot() {
  Reboot();
}

void PowerOptions::powerOff() {
  PowerOff();
}

}  // namespace backend
}  // namespace panel
