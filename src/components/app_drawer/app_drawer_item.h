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

#ifndef SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_ITEM_H_
#define SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_ITEM_H_

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QToolButton>

#include "src/info_server/application_info/application_info.h"

namespace panel {
namespace frontend {

class AppDrawerItem : public QWidget {
  Q_OBJECT

 public:
  explicit AppDrawerItem(const AppInfo& info, bool is_search_layout = false,
    QWidget * parent = nullptr);
  ~AppDrawerItem();
  AppInfo GetAppInfo();

 public slots:
  void UpdateFilter(const QString& keyword);

 private:
  QToolButton * btn_ = nullptr;
  QLabel * title_ = nullptr;
  AppInfo info_ = {};

 private slots:
  void Launch();
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_ITEM_H_
