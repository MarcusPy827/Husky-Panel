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
#include <QFile>
#include <QDir>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/application_services/application_services.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

void ApplicationServices::GetKRunner() {
  QDBusInterface interface(DBUS_KRUNNER_SERVICE, DBUS_KRUNNER_APP_PATH,
    DBUS_KRUNNER_APP_INTERFACE, QDBusConnection::sessionBus());

  if (!interface.isValid()) {
    LOG(WARNING) << absl::StrCat("Failed to connect to KRunner via D-Bus. ",
      "Is KRunner running? Doing nothing...");
    return;
  }

  QDBusReply<void> reply = interface.call("toggleDisplay");
  if (reply.isValid()) {
    LOG(INFO) << absl::StrCat("Successfully triggered KRunner via D-Bus!");
  } else {
    LOG(ERROR) << absl::StrCat("Failed to trigger KRunner:",
      reply.error().message().toStdString());
  }
}

}  // namespace backend
}  // namespace panel
