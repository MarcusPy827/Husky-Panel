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
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/session_handler/session_handler.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

SessionHandler::SessionHandler(QObject* parent) : QObject(parent) {
  LOG(INFO) << absl::StrCat("SessionHandler constructer invoked!!");
}

QDBusObjectPath SessionHandler::CurrentSessionPath() const {
  const QString session_id = qEnvironmentVariable("XDG_SESSION_ID");
  QDBusInterface manager(DBUS_LOGIND_SERVICE, DBUS_LOGIND_PATH,
    DBUS_LOGIND_MANAGER_INTERFACE, QDBusConnection::systemBus());
  const QDBusReply<QDBusObjectPath> reply = manager.call("GetSession",
    session_id);

  if (!reply.isValid()) {
    LOG(ERROR) << absl::StrCat(absl::StrFormat(
      "Failed to get current session path for session ID '%1' from "
      "logind via D-Bus: %2", session_id.toStdString(),
      reply.error().message().toStdString()));
    return QDBusObjectPath();
  }
  return reply.value();
}

void SessionHandler::Lock() {
  const QDBusObjectPath path = CurrentSessionPath();
  if (path.path().isEmpty()) {
    LOG(ERROR) << absl::StrCat("Failed to get current session path from ",
      "logind, cannot lock the screen.");
    return;
  }

  QDBusInterface session(DBUS_LOGIND_SERVICE, path.path(),
    DBUS_LOGIND_SESSION_INTERFACE, QDBusConnection::systemBus());
  session.call("Lock");
}

void SessionHandler::Logout() {
  /* Basically try each DE's session manager first for a graceful logout.
   * If failed, then call the logind Session.Teriminate() directly which kills
   * all process but could corrupt the desktop environment.
   */

  auto* bus = QDBusConnection::sessionBus().interface();
  if (bus) {
    /*
     * GNOME / Cinnamon / Unity
     * This software does NOT support GNOME due to Mutter does not implement
     * some of the protocol required, but really, I hope the bar could work
     * some day on GNOME as well, so this is kept.
     * 
     * Note: If gnome_int interface called with 1, which flags for no dialog.
     */

    if (bus->isServiceRegistered(DBUS_GNOME_SESSION_MANAGER_SERVICE)) {
      QDBusInterface gnome_int(DBUS_GNOME_SESSION_MANAGER_SERVICE,
        DBUS_GNOME_SESSION_MANAGER_PATH, DBUS_GNOME_SESSION_MANAGER_INTERFACE,
        QDBusConnection::sessionBus());
      gnome_int.asyncCall("Logout", static_cast<uint>(1));
      return;
    }

    /*
     * KDE Plasma
     * The target serivce "org.kde.Shutdown" is not always running but
     * activatable, so isServiceRegistered() always returns false.
     * Instead, we detect via XDG_CURRENT_DESKTOP and directly send the
     * message while D-Bus automatically activates the service if needed.
     */

    const bool is_kde = qEnvironmentVariable("XDG_CURRENT_DESKTOP")
      .contains("KDE", Qt::CaseInsensitive);
    if (is_kde) {
      auto kde_msg = QDBusMessage::createMethodCall(DBUS_KDE_SHUTDOWN_SERVICE,
        DBUS_KDE_SHUTDOWN_PATH, DBUS_KDE_SHUTDOWN_INTERFACE,
        DBUS_KDE_SHUTDOWN_LOGOUT_METHOD);
      QDBusConnection::sessionBus().asyncCall(kde_msg);
      return;
    }

    /*
     * XFCE
     *
     * Note: If xfce_int is called with false, false, then it means that
     * fast=false and showDialog=false.
     */

    if (bus->isServiceRegistered(DBUS_XFCE_SESSION_MANAGER_SERVICE)) {
      QDBusInterface xfce_int(DBUS_XFCE_SESSION_MANAGER_SERVICE,
        DBUS_XFCE_SESSION_MANAGER_PATH, DBUS_XFCE_SESSION_MANAGER_INTERFACE,
        QDBusConnection::sessionBus());
      xfce_int.asyncCall("Logout", false, false);
      return;
    }

    /*
     * Mate
     * 
     * Note: If mate_int interface called with 1, which flags for no dialog.
     */
    if (bus->isServiceRegistered(DBUS_MATE_SESSION_MANAGER_SERVICE)) {
      QDBusInterface mate_int(DBUS_MATE_SESSION_MANAGER_SERVICE,
        DBUS_MATE_SESSION_MANAGER_PATH, DBUS_MATE_SESSION_MANAGER_INTERFACE,
        QDBusConnection::sessionBus());
      mate_int.asyncCall("Logout", static_cast<uint>(1));
      return;
    }
  }

  // Fallback: ask logind to terminate the session via the manager. (forceful)
  const QDBusObjectPath session_path = CurrentSessionPath();
  if (!session_path.path().isEmpty()) {
    QDBusInterface session(DBUS_LOGIND_SERVICE, session_path.path(),
      DBUS_LOGIND_SESSION_INTERFACE, QDBusConnection::systemBus());
    session.call("Terminate");
  }
}

void SessionHandler::SwitchUser() {
  // Use the org.freedesktop.DisplayManager DBus interface (system bus).
  // This is the standard interface exposed by LightDM, SDDM, and GDM.
  QString seat_name = qEnvironmentVariable("XDG_SEAT", "seat0")
    .replace('-', '_');
  if (!seat_name.isEmpty()) {
    seat_name[0] = seat_name[0].toUpper();
  }

  const QString seat_path = QString(DBUS_DISPLAY_MANAGER_SEAT_PATH) + "/"
    + seat_name;

  QDBusInterface seat_int(DBUS_DISPLAY_MANAGER_SERVICE, seat_path,
    DBUS_DISPLAY_MANAGER_SEAT_INTERFACE, QDBusConnection::systemBus());
  seat_int.call("SwitchToGreeter");
}

void SessionHandler::lock() {
  Lock();
}

void SessionHandler::logout() {
  Logout();
}

void SessionHandler::switchUser() {
  SwitchUser();
}

}  // namespace backend
}  // namespace panel
