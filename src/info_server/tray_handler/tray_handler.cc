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

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QStringList>
#include <QDBusInterface>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/tray_handler/tray_handler.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

TrayHandler::TrayHandler() {
  LOG(INFO) << absl::StrCat("Connecting status notifier as host...");
  QString service_pid = "org.freedesktop.StatusNotifierHost-" +
    QString::number(QCoreApplication::applicationPid());
  QDBusConnection::sessionBus().registerService(service_pid);
  QDBusMessage call_watcher = QDBusMessage::createMethodCall(
    DBUS_STATUS_NOTIFIER_WATCHER_SERVICE, DBUS_STATUS_NOTIFIER_WATCHER_PATH,
    DBUS_STATUS_NOTIFIER_WATCHER_INTERFACE,
    DBUS_STATUS_NOTIFIER_WATCHER_METHOD);
  call_watcher << service_pid;
  QDBusConnection::sessionBus().call(call_watcher);

  QDBusConnection::sessionBus().connect(DBUS_STATUS_NOTIFIER_WATCHER_SERVICE,
    DBUS_STATUS_NOTIFIER_WATCHER_PATH, DBUS_STATUS_NOTIFIER_WATCHER_INTERFACE,
    DBUS_STATUS_NOTIFIER_WATCHER_ICON_REGISTERED_METHOD, this,
    SLOT(OnTrayIconRegistered(QString)));

  QDBusConnection::sessionBus().connect(DBUS_STATUS_NOTIFIER_WATCHER_SERVICE,
    DBUS_STATUS_NOTIFIER_WATCHER_PATH, DBUS_STATUS_NOTIFIER_WATCHER_INTERFACE,
    DBUS_STATUS_NOTIFIER_WATCHER_ICON_UNREGISTERED_METHOD, this,
    SLOT(OnTrayIconUnregistered(QString)));
}

TrayHandler::~TrayHandler() {
  LOG(INFO) << absl::StrCat("TrayHandler is being deleted.");
}

void TrayHandler::InitDataServer() {
  LOG(INFO) << absl::StrCat("Initializing QDBusServiceWatcher...");
  if (service_watcher_ == nullptr) {
    service_watcher_ = new QDBusServiceWatcher(this);
  }
  service_watcher_->setConnection(QDBusConnection::sessionBus());
  service_watcher_->setWatchMode(QDBusServiceWatcher::WatchForOwnerChange);
  QObject::connect(service_watcher_, &QDBusServiceWatcher::serviceOwnerChanged,
    this, [this](const QString &service, const QString &old,
        const QString &newOwner){
      if (newOwner.isEmpty()) {
        OnTrayIconUnregistered(service);
      }
    });

  QDBusInterface watcher(
    DBUS_STATUS_NOTIFIER_WATCHER_SERVICE,
    DBUS_STATUS_NOTIFIER_WATCHER_PATH,
    DBUS_STATUS_NOTIFIER_WATCHER_INTERFACE,
    QDBusConnection::sessionBus());

  if (watcher.isValid()) {
    QVariant items = watcher.property(
      DBUS_STATUS_NOTIFIER_WATCHER_GET_REGISTERED_METHOD);

    if (items.isValid()) {
      QStringList services = items.toStringList();
      LOG(INFO) << absl::StrFormat("Found %d existing icons.",
        services.count());
      for (const QString &service : services) {
        OnTrayIconRegistered(service);
      }
    } else {
      LOG(ERROR) << absl::StrCat(
        "Failed to read RegisteredStatusNotifierItems property.");
    }
  } else {
    LOG(ERROR) << absl::StrCat(
      "Failed to connect to StatusNotifierWatcher interface.");
  }
}

void TrayHandler::OnTrayIconRegistered(const QString& service) {
  service_watcher_->addWatchedService(service);
  LOG(INFO) << absl::StrCat("Emitting service append: ",
    service.toStdString());
  emit NewTrayIcon(service);
}

void TrayHandler::OnTrayIconUnregistered(const QString& service) {
  emit TrayIconDeleted(service);
}

}  // namespace backend
}  // namespace panel
