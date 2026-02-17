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

#ifndef SRC_COMPONENTS_SYSTEM_TRAY_SYSTEM_TRAY_H_
#define SRC_COMPONENTS_SYSTEM_TRAY_SYSTEM_TRAY_H_

#include <QWidget>
#include <QHBoxLayout>
#include <QSet>
#include <QHash>
#include <QString>
#include <QPushButton>

#include "src/info_server/tray_handler/tray_handler.h"
#include "src/components/system_tray/system_tray_icon_render.h"

namespace panel {
namespace frontend {

class SystemTray : public QWidget {
  Q_OBJECT

 public:
  explicit SystemTray(QWidget *parent = nullptr);
  ~SystemTray();

 private:
  void UpdateTrayIcons();

  QSet<QString> services_ = {};
  QHash<QString, SystemTrayIcon*> tray_icons_ = {};
  QHBoxLayout * layout_gen_ = nullptr;
  backend::TrayHandler * tray_handler_ = nullptr;

 private slots:
  void OnIconAdded(const QString& service);
  void OnIconDeleted(const QString& service);
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_SYSTEM_TRAY_SYSTEM_TRAY_H_
