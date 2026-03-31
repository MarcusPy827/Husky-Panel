/*
 * tray_icon_item.h
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

#ifndef SRC_COMPONENTS_SYSTEM_TRAY_TRAY_ICON_ITEM_H_
#define SRC_COMPONENTS_SYSTEM_TRAY_TRAY_ICON_ITEM_H_

#include <QObject>
#include <QString>
#include <QImage>
#include <QDBusInterface>

#include "lib/3rdparty/libdbusmenu/src/dbusmenuimporter.h"

namespace panel {
namespace frontend {

// Manages the D-Bus connection and icon data for a single StatusNotifierItem.
// Lives on the main thread; UpdateIcon/UpdateTooltip/UpdateStatus run there.
class TrayIconItem : public QObject {
  Q_OBJECT

 public:
  explicit TrayIconItem(const QString& service, QObject* parent = nullptr);
  ~TrayIconItem() = default;

  /* ---------- For backend usage ---------- */

  // Returns a copy of the current icon image (copy-safe for cross-thread use).
  QImage GetImage() const { return icon_image_; }
  QString GetTooltip() const { return tooltip_; }
  bool IsVisible() const { return status_ != QLatin1String("Passive"); }
  bool NeedsAttention() const {
    return status_ == QLatin1String("NeedsAttention");
  }

  void Activate(int x, int y);
  void ContextMenu(int x, int y);
  void SecondaryActivate(int x, int y);
  void Scroll(int delta);

 signals:
  void IconChanged();
  void TooltipChanged();
  void StatusChanged();

 private:
  QString real_service_;
  QString real_path_;

  QImage icon_image_;
  QString tooltip_;
  QString status_ = QStringLiteral("Active");

  QDBusInterface* dbus_ = nullptr;
  DBusMenuImporter* menu_importer_ = nullptr;

 private slots:
  void UpdateIcon();
  void UpdateTooltip();
  void UpdateStatus(const QString& status);
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_SYSTEM_TRAY_TRAY_ICON_ITEM_H_
