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

#ifndef SRC_COMPONENTS_SYSTEM_TRAY_SYSTEM_TRAY_ICON_RENDER_H_
#define SRC_COMPONENTS_SYSTEM_TRAY_SYSTEM_TRAY_ICON_RENDER_H_

#include <QWidget>
#include <QHBoxLayout>
#include <QSet>
#include <QHash>
#include <QString>
#include <QToolButton>
#include <QDBusInterface>

#include "lib/3rdparty/libdbusmenu/src/dbusmenuimporter.h"

namespace panel {
namespace frontend {

class SystemTrayIcon : public QWidget {
  Q_OBJECT

 public:
  explicit SystemTrayIcon(QString service, QWidget *parent = nullptr);
  ~SystemTrayIcon() = default;
  QToolButton * GetBtn();
  bool eventFilter(QObject *watched, QEvent *event) override;

 private:
  QString real_service_;
  QString real_path_;

  QToolButton * btn_ = nullptr;
  QDBusInterface * icon_dbus_interface_ = nullptr;
  DBusMenuImporter * menu_imported_ = nullptr;

 private slots:
  void UpdateIcon();
  void UpdateTooltip();
  void UpdateStatus(const QString& new_stat);

 protected:
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_SYSTEM_TRAY_SYSTEM_TRAY_ICON_RENDER_H_
