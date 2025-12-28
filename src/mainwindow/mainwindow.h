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

#include "src/quick_kde_su/quick_kde_su.h"
#include "src/clock/clock.h"

namespace panel {
namespace frontend {

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private:
  int inclusive_zone_height_ = 32;
  QRect screen_geometry_ = qApp->primaryScreen()->geometry();

  Ui::MainWindow * ui_;
  QuickKDESU * quick_kde_su_panel_ = nullptr;
  backend::Clock * clock_updater_ = nullptr;

 private slots:
  void TriggerKRunner();
  void TriggerQuickKDESUPanel();
  void UpdateBatteryPercentage();
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_MAINWINDOW_MAINWINDOW_H_
