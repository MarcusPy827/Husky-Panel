/*
 * system_tray_handler.h
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

#ifndef SRC_COMPONENTS_SYSTEM_TRAY_SYSTEM_TRAY_HANDLER_H_
#define SRC_COMPONENTS_SYSTEM_TRAY_SYSTEM_TRAY_HANDLER_H_

#include <QAbstractListModel>
#include <QHash>
#include <QImage>
#include <QList>
#include <QReadWriteLock>
#include <QString>

#include "src/info_server/tray_handler/tray_handler.h"
#include "src/components/system_tray/tray_icon_item.h"

namespace panel {
namespace frontend {

// QAbstractListModel that exposes the set of StatusNotifierItem icons to QML.
// Thread-safe icon image access for the rendering thread is handled via
// GetIconImage() which is protected by a QReadWriteLock.
class SystemTrayHandler : public QAbstractListModel {
  Q_OBJECT

 public:
  enum Roles {
    ServiceRole       = Qt::UserRole + 1,
    TooltipRole,
    IsVisibleRole,
    NeedsAttentionRole,
    IconRevisionRole,
    IconKeyRole,
  };

  explicit SystemTrayHandler(QObject* parent = nullptr);
  ~SystemTrayHandler() = default;

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index,
    int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  // Called by TrayIconImageProvider from the render thread.
  QImage GetIconImage(int key) const;

  /* ---------- For backend usage ---------- */
  void Activate(const QString& service, int x, int y);
  void ContextMenu(const QString& service, int x, int y);
  void SecondaryActivate(const QString& service, int x, int y);
  void Scroll(const QString& service, int delta);

  /* ---------- For QML usage ---------- */
  Q_INVOKABLE void activate(const QString& service, int x, int y);
  Q_INVOKABLE void contextMenu(const QString& service, int x, int y);
  Q_INVOKABLE void secondaryActivate(const QString& service, int x, int y);
  Q_INVOKABLE void scroll(const QString& service, int delta);

 private:
  int IndexOf(const QString& service) const;

  QList<QString> services_;
  QHash<QString, TrayIconItem*> entries_;
  QHash<QString, int> icon_revisions_;
  QHash<QString, int> icon_keys_;
  int next_key_ = 0;

  // Thread-safe image cache: written on main thread, read on render thread.
  mutable QReadWriteLock images_lock_;
  QHash<int, QImage> icon_images_;

  backend::TrayHandler* tray_handler_ = nullptr;

 private slots:
  void OnIconAdded(const QString& service);
  void OnIconDeleted(const QString& service);
  void OnEntryIconChanged(const QString& service);
  void OnEntryTooltipChanged(const QString& service);
  void OnEntryStatusChanged(const QString& service);
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_SYSTEM_TRAY_SYSTEM_TRAY_HANDLER_H_
