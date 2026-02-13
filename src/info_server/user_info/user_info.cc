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
#include <QFile>
#include <QDir>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

#include "src/info_server/user_info/user_info.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

QString UserInfo::GetUserName() {
  uid_t uid = getuid();
  QString result = "User Profile 1";
  QString user_path = QString("/org/freedesktop/Accounts/User%1").arg(uid);
  QDBusInterface interface(DBUS_ACCOUNTS_SERVICE, user_path,
    DBUS_ACCOUNTS_USER_INTERFACE, QDBusConnection::systemBus());
  if (interface.isValid()) {
    QVariant name_read = interface.property("RealName");
    if (name_read.isValid() && !name_read.toString().isEmpty()) {
      result = name_read.toString();
      qInfo() << "[ OK ] User Info: Successfully got user display name from"
        << "D-Bus.";
      return result;
    }
  }

  qWarning() << "[WARN] User Info: Failed to get user display name from"
    << "D-Bus. Now falling back to login name...";
  result = QString::fromLocal8Bit(qgetenv("USER"));
  return result;
}

QString UserInfo::GetUserAvatarPath() {
  uid_t uid = getuid();
  QString result;
  QString user_path = QString("/org/freedesktop/Accounts/User%1").arg(uid);
  QDBusInterface interface(DBUS_ACCOUNTS_SERVICE, user_path,
    DBUS_ACCOUNTS_USER_INTERFACE, QDBusConnection::systemBus());
  if (interface.isValid()) {
    QVariant avatar_read = interface.property("IconFile");
    if (avatar_read.isValid() && !avatar_read.toString().isEmpty()) {
      result = avatar_read.toString();
      if (QFile::exists(result)) {
        qInfo() << "[ OK ] User Info: Successfully got user avatar path from"
          << "D-Bus.";
        return result;
      }
    }
  }

  qWarning() << "[WARN] User Info: Failed to get user avatar path from"
    << "D-Bus. Now falling back to ~/.face.icon...";
  QString user_home = QDir::homePath();
  result = user_home + "/.face.icon";
  if (QFile::exists(result)) {
    qInfo() << "[ OK ] User Info: Successfully got user avatar path from"
      << "~/.face.icon.";
    return result;
  }

  qCritical() << "[ERROR] User Info: Failed to get user avatar path from"
    << "D-Bus and ~/.face.icon does not exist either, returning empty string.";
  result = "";
  return result;
}

}  // namespace backend
}  // namespace panel
