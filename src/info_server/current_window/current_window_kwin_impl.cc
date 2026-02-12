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

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/current_window/current_window_kwin_impl.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

CurrentWindowKwinImpl::CurrentWindowKwinImpl(QObject *parent) :
    CurrentWindowProvider(parent) {
  QDBusConnection bus = QDBusConnection::sessionBus();
  if (!bus.registerService(DBUS_CUSTOM_KWIN_WINDOW_SERVICE)) {
    LOG(ERROR) << absl::StrCat("Cannot register D-Bus service: ",
      bus.lastError().message().toStdString());
    return;
  }

  if (!bus.registerObject(DBUS_CUSTOM_KWIN_WINDOW_PATH, this,
      QDBusConnection::ExportAllSlots)) {
    LOG(ERROR) << absl::StrCat("Cannot register D-Bus object: ",
      bus.lastError().message().toStdString());
    return;
  }

  LOG(INFO) << absl::StrCat("App Indicator D-Bus service is up!!");
}

CurrentWindowKwinImpl::~CurrentWindowKwinImpl() {
  LOG(INFO) << absl::StrCat("CurrentWindowKwinImpl is being deleted...");
}

void CurrentWindowKwinImpl::UpdateActiveApp(QString name, QString appid,
    QString title) {
  LOG(INFO) << absl::StrCat("Detected active window changed!!");
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
