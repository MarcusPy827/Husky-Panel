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

#ifndef SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_SEARCH_ITEM_H_
#define SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_SEARCH_ITEM_H_

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QToolButton>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QShowEvent>

#include "src/info_server/application_info/application_info.h"

namespace panel {
namespace frontend {

class AppDrawerSearchItem : public QWidget {
  Q_OBJECT

 public:
  explicit AppDrawerSearchItem(const AppInfo& info, QWidget * parent = nullptr);
  ~AppDrawerSearchItem();
  AppInfo GetAppInfo();

 public slots:
  void UpdateFilter(const QString& keyword);

 protected:
  void mousePressEvent(QMouseEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void showEvent(QShowEvent *event) override;

 private:
  QToolButton * btn_ = nullptr;
  QLabel * title_ = nullptr;
  QLabel * comment_ = nullptr;
  AppInfo info_ = {};
  QString raw_comment_;

  void UpdateCommentElide();

 private slots:
  void Launch();
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_SEARCH_ITEM_H_
