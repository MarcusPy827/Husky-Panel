/*
 * Copyright (C) 2025 MarcusPy827
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

#ifndef SRC_MAINWINDOW_MAINWINDOW_H_
#define SRC_MAINWINDOW_MAINWINDOW_H_

#include <QWidget>
#include <QScreen>
#include <QHBoxLayout>
#include <QTimer>

#include "src/components/app_indicator/app_indicator.h"
#include "src/components/app_drawer/app_drawer_btn.h"
#include "src/components/krunner_btn/krunner_btn.h"
#include "src/components/battery_indicator/battery_indicator.h"
#include "src/components/clock_btn/clock_btn.h"
#include "src/theme_loader/theme_loader.h"

namespace panel {
namespace frontend {

class MainWindow : public QWidget {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private:
  void LoadLeftSlot();
  void LoadMiddleSlot();
  void LoadRightSlot();
  void HandleTheme();
  int inclusive_zone_height_ = 32;
  QRect screen_geometry_ = qApp->primaryScreen()->geometry();
  QTimer * update_timer_ = nullptr;

  QWidget * base_layer_ = nullptr;
  QHBoxLayout * slot_left_ = nullptr;
  QHBoxLayout * slot_middle_ = nullptr;
  QHBoxLayout * slot_right_ = nullptr;

  AppIndicator * app_indicator_ = nullptr;
  AppDrawerBtn * app_drawer_btn_ = nullptr;
  KRunnerBtn * krunner_btn_ = nullptr;
  BatteryIndicator * battery_indicator_ = nullptr;
  ClockBtn * clock_btn_ = nullptr;
  loader::ThemeLoader * theme_loader_ = nullptr;

 private slots:
  void TriggerKRunner();
  void TriggerQuickKDESUPanel();
  void UpdateWlanSignalStrength();
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_MAINWINDOW_MAINWINDOW_H_
