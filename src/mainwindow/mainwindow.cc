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

#include <QWindow>
#include <QDebug>
#include <QTimer>

#include "src/mainwindow/mainwindow.h"
#include "src/mainwindow/ui_mainwindow.h"
#include "src/user_info/user_info.h"
#include "src/application_services/application_services.h"
#include "src/battery_info/battery_info.h"
#include "src/utils/utils.h"
#include "lib/3rdparty/layer-shell-qt/src/interfaces/window.h"

namespace panel {
namespace frontend {

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui_(new
    Ui::MainWindow) {
  ui_->setupUi(this);
  setWindowTitle(tr("Top Panel"));
  setFixedHeight(inclusive_zone_height_);
  setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

  winId();
  QWindow * native_handle = windowHandle();
  if (native_handle) {
    LayerShellQt::Window * layer_shell = LayerShellQt::Window::get(
      native_handle);
    layer_shell->setAnchors(static_cast<LayerShellQt::Window::Anchor>(
      LayerShellQt::Window::Anchor::AnchorTop |
      LayerShellQt::Window::Anchor::AnchorLeft |
      LayerShellQt::Window::Anchor::AnchorRight));
    layer_shell->setLayer(LayerShellQt::Window::LayerTop);
    layer_shell->setKeyboardInteractivity(LayerShellQt::Window::
      KeyboardInteractivityNone);
    layer_shell->setExclusiveZone(inclusive_zone_height_);
  } else {
    qFatal() << tr(
      "Failed to get native window handle. Please check your session,");
    return;
  }

  qInfo() << "[INFO] Top Bar: Now initializing user info...";
  ui_->user_name_btn->setText(backend::UserInfo::GetUserName());
  ui_->user_name_btn->setVisible(false);
  ui_->avatar_btn->setIcon(
    QIcon(backend::UserInfo::GetUserAvatarPath()));
  ui_->avatar_btn->setVisible(false);

  connect(ui_->krunner_btn, &QPushButton::clicked, this,
    &MainWindow::TriggerKRunner);

  qInfo() << "[INFO] Top Bar: Initializing QuickKDESU panel...";
  quick_kde_su_panel_ = new QuickKDESU(nullptr);
  quick_kde_su_panel_->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
  connect(ui_->quick_kdesu_btn, &QPushButton::clicked, this,
    &MainWindow::TriggerQuickKDESUPanel);

  qInfo() << "[INFO] Top Bar: Initializing clock updater...";
  clock_updater_ = new backend::Clock(ui_->clock_btn);

  qInfo() << "[INFO] Top Bar: Initializing internal updater...";
  QTimer * timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this,
    &MainWindow::UpdateBatteryPercentage);
  timer->start(1000);

  qInfo() << "[ OK ] Top Bar: Initialization complete.";
}

MainWindow::~MainWindow() {
  delete ui_;
  delete quick_kde_su_panel_;
  delete clock_updater_;
  qInfo() << "[ OK ] Top Bar: Successfully cleaned up main window.";
}

void MainWindow::TriggerKRunner() {
  backend::ApplicationServices::GetKRunner();
}

void MainWindow::TriggerQuickKDESUPanel() {
  if (quick_kde_su_panel_->isVisible()) {
    qInfo() << "[INFO] Top Bar: Now closing quick KDESU panel...";
    quick_kde_su_panel_->hide();
  } else {
    qInfo() << "[INFO] Top Bar: Now opening quick KDESU panel...";
    quick_kde_su_panel_->show();
    quick_kde_su_panel_->move(screen_geometry_.left() + ui_->
      quick_kdesu_btn->x() - 9, 32);
  }
}

void MainWindow::UpdateBatteryPercentage() {
  int battery_level = backend::BatteryInfo::GetBatteryLevel();
  QString battery_icon_name;
  switch (battery_level) {
    case 0 ... 20:
      battery_icon_name = "alert";
      break;

    case 21 ... 40:
      battery_icon_name = "30";
      break;

    case 41 ... 50:
      battery_icon_name = "40";
      break;

    case 51 ... 75:
      battery_icon_name = "75";
      break;

    case 76 ... 90:
      battery_icon_name = "80";
      break;

    case 91 ... 100:
      battery_icon_name = "full";
      break;

    default:
      battery_icon_name = "alert";
      break;
  }
  QString icon_path = utils::Utils::TemplateCat(QStringLiteral(
    """:/icons/icons/3rdparty/material-symbols/bat_%t1%.svg"""),
    QList<QString>{battery_icon_name});
  ui_->battery_btn->setIcon(QIcon(icon_path));
}

}  // namespace frontend
}  // namespace panel
