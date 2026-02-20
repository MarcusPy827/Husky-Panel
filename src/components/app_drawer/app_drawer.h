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

#ifndef SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_H_
#define SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_H_

#include <QWidget>
#include <QToolButton>
#include <QLineEdit>

#include "src/theme_loader/theme_loader.h"
#include "src/components/app_drawer/app_drawer_side_pane_item.h"

namespace panel {
namespace frontend {

class AppDrawer : public QWidget {
  Q_OBJECT

 public:
  explicit AppDrawer(QWidget *parent = nullptr);
  ~AppDrawer();

 private:
  QToolButton * menu_btn_ = nullptr;
  QLineEdit * search_bar_internal_ = nullptr;
  QToolButton * search_bar_icon_btn_internal_ = nullptr;
  QToolButton * avatar_btn_ = nullptr;
  QToolButton * sleep_btn_ = nullptr;
  QToolButton * hibernate_btn_ = nullptr;
  QToolButton * restart_btn_ = nullptr;
  QToolButton * shut_down_btn_ = nullptr;
  QToolButton * session_btn_ = nullptr;
  QWidget * side_pane_ = nullptr;
  AppDrawerSidePaneItem * all_apps_btn_ = nullptr;
  AppDrawerSidePaneItem * audio_video_apps_btn_ = nullptr;
  AppDrawerSidePaneItem * development_apps_btn_ = nullptr;
  AppDrawerSidePaneItem * education_apps_btn_ = nullptr;
  AppDrawerSidePaneItem * game_apps_btn_ = nullptr;
  AppDrawerSidePaneItem * graphics_apps_btn_ = nullptr;
  AppDrawerSidePaneItem * network_apps_btn_ = nullptr;
  AppDrawerSidePaneItem * office_apps_btn_ = nullptr;
  AppDrawerSidePaneItem * settings_apps_btn_ = nullptr;
  AppDrawerSidePaneItem * system_apps_btn_ = nullptr;
  AppDrawerSidePaneItem * utility_apps_btn_ = nullptr;

  loader::ThemeLoader * theme_loader_ = nullptr;
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_H_
