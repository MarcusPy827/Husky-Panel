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
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QTimer>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/mainwindow/mainwindow.h"
#include "src/utils/color_palette_wrapper/color_palette_wrapper.h"
#include "src/user_info/user_info.h"
#include "src/application_services/application_services.h"
#include "src/battery_info/battery_info.h"
#include "src/wlan_info/wlan_info.h"
#include "src/utils/utils.h"

#include "lib/3rdparty/layer-shell-qt/src/interfaces/window.h"

namespace panel {
namespace frontend {

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
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

  this->setObjectName("panel_container");
  this->setStyleSheet(
    "#panel_container { background: #000000 }");

  QHBoxLayout * container_layout = new QHBoxLayout();
  container_layout->setContentsMargins(0, 0, 0, 0);
  container_layout->setSpacing(0);
  this->setLayout(container_layout);

  if (base_layer_ == nullptr) {
    base_layer_ = new QWidget();
    base_layer_->setProperty("class", "base_layer");
  }
  container_layout->addWidget(base_layer_);

  QHBoxLayout * bar_layout = new QHBoxLayout();
  bar_layout->setContentsMargins(8, 0, 8, 0);
  bar_layout->setSpacing(6);
  base_layer_->setLayout(bar_layout);

  if (slot_left_ == nullptr) {
    slot_left_ = new QHBoxLayout();
    slot_left_->setContentsMargins(0, 0, 0, 0);
    slot_left_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    slot_left_->setSpacing(0);
  }
  bar_layout->addLayout(slot_left_);

  QSpacerItem * bar_spacer_1 = new QSpacerItem(8, 8, QSizePolicy::Expanding,
    QSizePolicy::Expanding);
  bar_layout->addSpacerItem(bar_spacer_1);

  if (slot_middle_ == nullptr) {
    slot_middle_ = new QHBoxLayout();
    slot_middle_->setContentsMargins(0, 0, 0, 0);
    slot_middle_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    slot_middle_->setSpacing(0);
  }
  bar_layout->addLayout(slot_middle_);

  QSpacerItem * bar_spacer_2 = new QSpacerItem(8, 8, QSizePolicy::Expanding,
    QSizePolicy::Expanding);
  bar_layout->addSpacerItem(bar_spacer_2);

  if (slot_right_ == nullptr) {
    slot_right_ = new QHBoxLayout();
    slot_right_->setContentsMargins(0, 0, 0, 0);
    slot_right_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    slot_right_->setSpacing(0);
  }
  bar_layout->addLayout(slot_right_);

  LoadLeftSlot();
  LoadMiddleSlot();
  LoadRightSlot();
  HandleTheme();

  /*
  QHBoxLayout * widget_slot_front_layout = new QHBoxLayout();
  widget_slot_front_layout->setContentsMargins(0, 0, 0, 0);
  widget_slot_front_layout->setSpacing(8);
  // ui_->widget_slot_front->setLayout(widget_slot_front_layout);

  if (app_indicator_ == nullptr) {
    qInfo() << "[INFO] Top Bar: Initializing app indicator...";
    app_indicator_ = new AppIndicator();
    widget_slot_front_layout->addWidget(app_indicator_);
  }

  qInfo() << "[INFO] Top Bar: Now initializing user info...";
  // ui_->user_name_btn->setText(backend::UserInfo::GetUserName());
  // ui_->user_name_btn->setVisible(false);
  // ui_->avatar_btn->setIcon(
    QIcon(backend::UserInfo::GetUserAvatarPath()));
  // ui_->avatar_btn->setVisible(false);

  connect(// ui_->krunner_btn, &QPushButton::clicked, this,
    &MainWindow::TriggerKRunner);

  qInfo() << "[INFO] Top Bar: Initializing QuickKDESU panel...";
  quick_kde_su_panel_ = new QuickKDESU(nullptr);
  quick_kde_su_panel_->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
  connect(// ui_->quick_kdesu_btn, &QPushButton::clicked, this,
    &MainWindow::TriggerQuickKDESUPanel);

  qInfo() << "[INFO] Top Bar: Initializing clock updater...";
  clock_updater_ = new backend::Clock(// ui_->clock_btn);

  qInfo() << "[INFO] Top Bar: Initializing internal updater...";
  QTimer * timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this,
    &MainWindow::UpdateBatteryPercentage);
  connect(timer, &QTimer::timeout, this,
    &MainWindow::UpdateWlanSignalStrength);
  timer->start(1000);

  qInfo() << "[ OK ] Top Bar: Initialization complete.";
  */
}

MainWindow::~MainWindow() {
  //delete quick_kde_su_panel_;
  //delete clock_updater_;
  qInfo() << "[ OK ] Top Bar: Successfully cleaned up main window.";
}

void MainWindow::HandleTheme() {
  QString highlight_color = utils::ColorPaletteWrapper::
    GetSystemHighlightColor().name();
  if (theme_loader_ == nullptr) {
    theme_loader_ = new loader::ThemeLoader(base_layer_, highlight_color, this);
  }
}

void MainWindow::LoadLeftSlot() {
  if (app_indicator_ == nullptr) {
    LOG(INFO) << absl::StrCat("Now loading application indicator...");
    app_indicator_ = new AppIndicator();
    slot_left_->addWidget(app_indicator_);
  }
}

void MainWindow::LoadMiddleSlot() {

}

void MainWindow::LoadRightSlot() {
  if (clock_btn_ == nullptr) {
    LOG(INFO) << absl::StrCat("Now loading clock...");
    clock_btn_ = new ClockBtn();
    slot_right_->addWidget(clock_btn_);
  }
}

void MainWindow::TriggerKRunner() {
  backend::ApplicationServices::GetKRunner();
}

void MainWindow::TriggerQuickKDESUPanel() {
  /*
  if (quick_kde_su_panel_->isVisible()) {
    qInfo() << "[INFO] Top Bar: Now closing quick KDESU panel...";
    quick_kde_su_panel_->hide();
  } else {
    qInfo() << "[INFO] Top Bar: Now opening quick KDESU panel...";
    quick_kde_su_panel_->show();
    quick_kde_su_panel_->move(screen_geometry_.left() + // ui_->
      quick_kdesu_btn->x() - 9, 32);
  }
      */
}

void MainWindow::UpdateBatteryPercentage() {
  /*
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
      battery_icon_name = "90";
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
  // ui_->battery_btn->setIcon(QIcon(icon_path));
  */
}

void MainWindow::UpdateWlanSignalStrength() {
  /*
  int signal_strength = backend::WlanInfo::GetWlanSignalStrength();
  QString wlan_icon_name;
  switch (signal_strength) {
    case -1:
      wlan_icon_name = "off";
      break;

    case 0 ... 25:
      wlan_icon_name = "1";
      break;

    case 26 ... 50:
      wlan_icon_name = "2";
      break;

    case 51 ... 75:
      wlan_icon_name = "3";
      break;

    case 76 ... 100:
      wlan_icon_name = "full";
      break;

    default:
      wlan_icon_name = "0";
      break;
  }
  QString icon_path = utils::Utils::TemplateCat(QStringLiteral(
    """:/icons/icons/3rdparty/material-symbols/wifi_%t1%.svg"""),
    QList<QString>{wlan_icon_name});
  // // ui_->wlan_btn->setIcon(QIcon(icon_path));
  */
}

}  // namespace frontend
}  // namespace panel
