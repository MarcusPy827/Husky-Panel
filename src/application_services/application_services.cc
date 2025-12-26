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
#include <QDebug>
#include <QFile>
#include <QDir>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

#include "src/application_services/application_services.h"

namespace panel {
namespace backend {

void ApplicationServices::GetKRunner() {
  QDBusInterface interface("org.kde.krunner", "/App", "org.kde.krunner.App",
    QDBusConnection::sessionBus());

  if (!interface.isValid()) {
    qWarning() << "[WARN] Application services: Failed to connect to KRunner"
      << "via D-Bus, doing nothing...";
    return;
  }

  QDBusReply<void> reply = interface.call("toggleDisplay");
  if (reply.isValid()) {
    qInfo() << "[ OK ] Application services: Successfully triggered KRunner!";
  } else {
    qCritical() << "[ERROR] Application services: Failed to trigger"
      << "KRunner:" << reply.error().message();
  }
}

}  // namespace backend
}  // namespace panel
