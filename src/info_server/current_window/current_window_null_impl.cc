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
#include <QDBusReply>
#include <QDBusMessage>

#include "src/info_server/current_window/current_window_null_impl.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

CurrentWindowNullImpl::CurrentWindowNullImpl(QObject *parent) :
    CurrentWindowProvider(parent) {
  qCritical() << "[ERROR] CurrentWindowNullImpl: Current WM is not"
    << "supported. Current window info server could NOT be initialized.";
}

CurrentWindowNullImpl::~CurrentWindowNullImpl() {
  qInfo() << "[INFO] CurrentWindowNullImpl: CurrentWindowNullImpl is being"
    << "deleted.";
}

QString CurrentWindowNullImpl::GetApplicationName() {
  return "";
}

QString CurrentWindowNullImpl::GetPackageName() {
  return "U";
}

}  // namespace backend
}  // namespace panel
