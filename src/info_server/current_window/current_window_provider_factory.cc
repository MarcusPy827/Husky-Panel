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

#include <memory>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDebug>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/current_window/current_window_provider_factory.h"
#include "src/info_server/current_window/current_window_kwin_impl.h"
#include "src/info_server/current_window/current_window_null_impl.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

std::unique_ptr<CurrentWindowProvider>InitCurrentWindowInfoServer(
    QObject *parent) {
  QDBusConnectionInterface * session_bus = QDBusConnection::sessionBus()
    .interface();
  bool is_kwin = session_bus && session_bus->isServiceRegistered(
    DBUS_KWIN_SERVICE);

  if (is_kwin) {
    return std::make_unique<CurrentWindowKwinImpl>(parent);
  }

  LOG(ERROR) << absl::StrCat("Current WM is NOT supported. ",
    "Current window info server could NOT be initialized.");
  return std::make_unique<CurrentWindowNullImpl>(parent);
}

}  // namespace backend
}  // namespace panel
