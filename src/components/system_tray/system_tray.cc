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

#include <QList>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/components/system_tray/system_tray.h"

namespace panel {
namespace frontend {

SystemTray::SystemTray(QWidget *parent) : QWidget(parent) {
  if (layout_gen_ == nullptr) {
    LOG(INFO) << absl::StrCat("Initializing system tray layout...");
    layout_gen_ = new QHBoxLayout();
  }
  layout_gen_->setContentsMargins(0, 0, 0, 0);
  layout_gen_->setSpacing(6);
  setLayout(layout_gen_);

  if (tray_handler_ == nullptr) {
    LOG(INFO) << absl::StrCat("Initializing system tray handler...");
    tray_handler_ = new backend::TrayHandler();
  }

  QObject::connect(tray_handler_, &backend::TrayHandler::NewTrayIcon, this,
    &SystemTray::OnIconAdded);
  QObject::connect(tray_handler_, &backend::TrayHandler::TrayIconDeleted, this,
    &SystemTray::OnIconDeleted);
  tray_handler_->InitDataServer();
}

SystemTray::~SystemTray() {
  LOG(INFO) << absl::StrCat("SystemTray is being deleted.");
}

void SystemTray::OnIconAdded(const QString& service) {
  LOG(INFO) << absl::StrCat("Detected system tray icon has been added.");
  if (services_.contains(service)) {
    LOG(WARNING) << absl::StrCat("The service '", service.toStdString(),
      "' is already contained in current list, skipping...");
    return;
  }
  services_.insert(service);
  UpdateTrayIcons();
}

void SystemTray::OnIconDeleted(const QString& service) {
  if (!services_.contains(service)) {
    LOG(WARNING) << absl::StrCat("Wired, the services'", service.toStdString(),
    "' is already NO LONGER EXISTS in the list, doing nothing...");
    return;
  }
  services_.remove(service);
  UpdateTrayIcons();
}

void SystemTray::UpdateTrayIcons() {
  LOG(INFO) << absl::StrCat("Updating tray icons...");
  for (auto i = services_.begin(); i != services_.end(); ++i) {
    if (!tray_icons_.contains(*i)) {
      SystemTrayIcon * icon_gen = new SystemTrayIcon(*i);
      layout_gen_->addWidget(icon_gen);
      tray_icons_.insert(*i, icon_gen);
    }
  }

  QList<QString> pending_remove = {};
  for (auto i = tray_icons_.begin(); i != tray_icons_.end(); ++i) {
    if (!services_.contains(i.key())) {
      layout_gen_->removeWidget(i.value());
      delete i.value();
      pending_remove.append(i.key());
    }
  }

  for (auto i = pending_remove.begin(); i != pending_remove.end(); ++i) {
    tray_icons_.remove(*i);
  }
}

}  // namespace frontend
}  // namespace panel
