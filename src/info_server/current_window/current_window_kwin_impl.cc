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

#include <KService>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMessage>

#include "src/info_server/current_window/current_window_kwin_impl.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

CurrentWindowKwinImpl::CurrentWindowKwinImpl(QObject *parent) :
    CurrentWindowProvider(parent) {
  QDBusConnection bus = QDBusConnection::sessionBus();
  if (!bus.registerService(DBUS_CUSTOM_KWIN_WINDOW_SERVICE)) {
    qCritical() << "[ERROR] Cannot regester D-Bus service: "
      << bus.lastError().message();
  }

  if (!bus.registerObject(DBUS_CUSTOM_KWIN_WINDOW_PATH, this,
      QDBusConnection::ExportAllSlots)) {
    qCritical() << "[ERROR] Cannot regester D-Bus object: "
      << bus.lastError().message();
  }
}

CurrentWindowKwinImpl::~CurrentWindowKwinImpl() {
  qInfo() << "[INFO] CurrentWindowKwinImpl: CurrentWindowKwinImpl is being"
    << "deleted.";
}

void CurrentWindowKwinImpl::UpdateActiveApp(QString name, QString appid,
    QString title) {
  KService::Ptr service = KService::serviceByDesktopName(name);
  if (!service) {
    service = KService::serviceByDesktopName(appid.toLower());
  }

  if (service) {
    window_info_.application_name = service->name();
  } else {
    window_info_.application_name = name;
  }

  window_info_.package_name = appid;
  emit CurrentWindowChanged();
}

void CurrentWindowKwinImpl::OnActiveWindowChanged() {
  emit CurrentWindowChanged();
}

QString CurrentWindowKwinImpl::GetApplicationName() {
  return window_info_.application_name;
}

QString CurrentWindowKwinImpl::GetPackageName() {
  return window_info_.package_name;
}

}  // namespace backend
}  // namespace panel
