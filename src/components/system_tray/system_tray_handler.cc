/*
 * system_tray_handler.cc
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

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"

#include "src/components/system_tray/system_tray_handler.h"
#include "src/config_loader/path_finder/path_finder.h"

namespace panel {
namespace frontend {

SystemTrayHandler::SystemTrayHandler(QObject* parent)
    : QAbstractListModel(parent) {
  LOG(INFO) << absl::StrCat("Initializing SystemTrayHandler...");
  settings_ = new QSettings(loader::PathFinder::GetTrayConfigPath(),
    QSettings::IniFormat, this);
  tray_handler_ = new backend::TrayHandler();
  QObject::connect(tray_handler_, &backend::TrayHandler::NewTrayIcon,
    this, &SystemTrayHandler::OnIconAdded);
  QObject::connect(tray_handler_, &backend::TrayHandler::TrayIconDeleted,
    this, &SystemTrayHandler::OnIconDeleted);
  tray_handler_->InitDataServer();
}

int SystemTrayHandler::rowCount(const QModelIndex& parent) const {
  if (parent.isValid()) return 0;
  return services_.size();
}

QVariant SystemTrayHandler::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= services_.size())
    return QVariant();

  const QString& service = services_.at(index.row());
  TrayIconItem* entry = entries_.value(service, nullptr);
  if (!entry) return QVariant();

  switch (role) {
    case ServiceRole:        return service;
    case RealServiceRole:    return entry->GetRealService();
    case TitleRole:          return entry->GetTitle();
    case AppIdRole:          return entry->GetAppId();
    case TooltipRole:        return entry->GetTooltip();
    case IsVisibleRole:      return entry->IsVisible();
    case IsConfigVisibleRole: return !user_hidden_.value(service, false);
    case NeedsAttentionRole: return entry->NeedsAttention();
    case IconRevisionRole:   return icon_revisions_.value(service, 0);
    case IconKeyRole:        return icon_keys_.value(service, -1);
    default:                 return QVariant();
  }
}

QHash<int, QByteArray> SystemTrayHandler::roleNames() const {
  return {
    { ServiceRole,        "service"        },
    { RealServiceRole,    "real_service"   },
    { TitleRole,          "title"          },
    { AppIdRole,          "app_id"         },
    { TooltipRole,        "tooltip"        },
    { IsVisibleRole,       "is_visible"      },
    { IsConfigVisibleRole, "is_config_visible" },
    { NeedsAttentionRole,  "needs_attention" },
    { IconRevisionRole,   "icon_revision"  },
    { IconKeyRole,        "icon_key"       },
  };
}

QImage SystemTrayHandler::GetIconImage(int key) const {
  QReadLocker locker(&images_lock_);
  return icon_images_.value(key);
}

void SystemTrayHandler::Activate(const QString& service, int x, int y) {
  if (auto* e = entries_.value(service, nullptr)) e->Activate(x, y);
}

void SystemTrayHandler::ContextMenu(const QString& service, int x, int y) {
  if (auto* e = entries_.value(service, nullptr)) e->ContextMenu(x, y);
}

void SystemTrayHandler::SecondaryActivate(
    const QString& service, int x, int y) {
  if (auto* e = entries_.value(service, nullptr)) e->SecondaryActivate(x, y);
}

void SystemTrayHandler::Scroll(const QString& service, int delta) {
  if (auto* e = entries_.value(service, nullptr)) e->Scroll(delta);
}

void SystemTrayHandler::activate(const QString& service, int x, int y) {
  Activate(service, x, y);
}

void SystemTrayHandler::contextMenu(const QString& service, int x, int y) {
  ContextMenu(service, x, y);
}

void SystemTrayHandler::secondaryActivate(
    const QString& service, int x, int y) {
  SecondaryActivate(service, x, y);
}

void SystemTrayHandler::scroll(const QString& service, int delta) {
  Scroll(service, delta);
}

void SystemTrayHandler::setConfigVisible(const QString& service, bool visible) {
  int row = IndexOf(service);
  if (row == -1) return;
  user_hidden_[service] = !visible;

  TrayIconItem* entry = entries_.value(service, nullptr);
  if (entry) {
    settings_->setValue(entry->GetRealService() + "/visible", visible);
    settings_->sync();
  }

  QModelIndex idx = index(row);
  emit dataChanged(idx, idx, {IsConfigVisibleRole});
}

void SystemTrayHandler::OnIconAdded(const QString& service) {
  if (entries_.contains(service)) {
    LOG(WARNING) << absl::StrCat("OnIconAdded: '", service.toStdString(),
      "' already tracked, skipping.");
    return;
  }
  LOG(INFO) << absl::StrCat("OnIconAdded: '", service.toStdString(), "'.");

  int key = next_key_++;
  icon_keys_[service] = key;
  icon_revisions_[service] = 0;

  auto* entry = new TrayIconItem(service, this);

  // Load persisted visibility preference (default: visible).
  {
    const QString cfg_key = entry->GetRealService();
    bool visible = settings_->value(cfg_key + "/visible", true).toBool();
    if (!visible) user_hidden_[service] = true;
  }

  // Cache initial icon image.
  {
    QWriteLocker locker(&images_lock_);
    icon_images_[key] = entry->GetImage();
  }

  // Wire up change signals. Use lambda to capture service name.
  QObject::connect(entry, &TrayIconItem::IconChanged, this,
    [this, service]() { OnEntryIconChanged(service); });
  QObject::connect(entry, &TrayIconItem::TooltipChanged, this,
    [this, service]() { OnEntryTooltipChanged(service); });
  QObject::connect(entry, &TrayIconItem::StatusChanged, this,
    [this, service]() { OnEntryStatusChanged(service); });

  int row = services_.size();
  beginInsertRows(QModelIndex(), row, row);
  services_.append(service);
  entries_[service] = entry;
  endInsertRows();
}

void SystemTrayHandler::OnIconDeleted(const QString& service) {
  int row = IndexOf(service);
  if (row == -1) {
    LOG(WARNING) << absl::StrCat("OnIconDeleted: '", service.toStdString(),
      "' not in list, skipping.");
    return;
  }
  LOG(INFO) << absl::StrCat("OnIconDeleted: '", service.toStdString(), "'.");

  int key = icon_keys_.value(service, -1);

  beginRemoveRows(QModelIndex(), row, row);
  services_.removeAt(row);
  delete entries_.take(service);
  icon_revisions_.remove(service);
  icon_keys_.remove(service);
  user_hidden_.remove(service);
  endRemoveRows();

  if (key >= 0) {
    QWriteLocker locker(&images_lock_);
    icon_images_.remove(key);
  }
}

void SystemTrayHandler::OnEntryIconChanged(const QString& service) {
  int row = IndexOf(service);
  if (row == -1) return;

  TrayIconItem* entry = entries_.value(service, nullptr);
  if (!entry) return;

  int key = icon_keys_.value(service, -1);
  if (key >= 0) {
    QWriteLocker locker(&images_lock_);
    icon_images_[key] = entry->GetImage();
  }

  icon_revisions_[service]++;
  QModelIndex idx = index(row);
  emit dataChanged(idx, idx, { IconRevisionRole });
}

void SystemTrayHandler::OnEntryTooltipChanged(const QString& service) {
  int row = IndexOf(service);
  if (row == -1) return;
  QModelIndex idx = index(row);
  emit dataChanged(idx, idx, { TooltipRole });
}

void SystemTrayHandler::OnEntryStatusChanged(const QString& service) {
  int row = IndexOf(service);
  if (row == -1) return;
  QModelIndex idx = index(row);
  emit dataChanged(idx, idx, { IsVisibleRole, NeedsAttentionRole });
}

int SystemTrayHandler::IndexOf(const QString& service) const {
  return services_.indexOf(service);
}

}  // namespace frontend
}  // namespace panel
