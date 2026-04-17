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
#include "src/info_server/current_window/current_window_wlroots_impl.h"
#include "src/info_server/current_window/current_window_xorg_impl.h"
#include "src/info_server/current_window/current_window_null_impl.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

std::unique_ptr<CurrentWindowProvider>InitCurrentWindowInfoServer(
    QObject *parent) {
  bool is_wayland = !qEnvironmentVariable("WAYLAND_DISPLAY").isEmpty();
  bool is_xorg = !qEnvironmentVariable("DISPLAY").isEmpty();

  QDBusConnectionInterface * session_bus = QDBusConnection::sessionBus()
    .interface();
  bool is_kwin = session_bus && session_bus->isServiceRegistered(
    DBUS_KWIN_SERVICE);

  // The KWin D-Bus script path is only needed on Wayland+KWin because XCB
  // cannot inspect windows there. On X11 (even with KWin, e.g. GXDE/DDE),
  // the XCB impl is self-contained and does not require the app-bridge script.
  if (is_kwin && is_wayland && !is_xorg) {
    LOG(INFO) << absl::StrCat("KWin Wayland session detected. ",
      "Using KWin D-Bus app-bridge script.");
    return std::make_unique<CurrentWindowKwinImpl>(parent);
  }

  if (is_wayland && !is_xorg) {
    LOG(INFO) << absl::StrCat("Non-KWin Wayland session detected. ",
      "Using wlr-foreign-toplevel.");
    return std::make_unique<CurrentWindowWlrootsImpl>(parent);
  }

  if (is_xorg) {
    LOG(INFO) << absl::StrCat("X11/XOrg session detected. ",
      "Using _NET_ACTIVE_WINDOW via XCB.");
    return std::make_unique<CurrentWindowXorgImpl>(parent);
  }

  LOG(ERROR) << absl::StrCat("Current WM is NOT supported. ",
    "Current window info server could NOT be initialized.");
  return std::make_unique<CurrentWindowNullImpl>(parent);
}

}  // namespace backend
}  // namespace panel
