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
#include "src/application_services/application_services.h"

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
    slot_left_->setSpacing(4);
  }
  bar_layout->addLayout(slot_left_);

  QSpacerItem * bar_spacer_1 = new QSpacerItem(8, 8, QSizePolicy::Expanding,
    QSizePolicy::Expanding);
  bar_layout->addSpacerItem(bar_spacer_1);

  if (slot_middle_ == nullptr) {
    slot_middle_ = new QHBoxLayout();
    slot_middle_->setContentsMargins(0, 0, 0, 0);
    slot_middle_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    slot_middle_->setSpacing(4);
  }
  bar_layout->addLayout(slot_middle_);

  QSpacerItem * bar_spacer_2 = new QSpacerItem(8, 8, QSizePolicy::Expanding,
    QSizePolicy::Expanding);
  bar_layout->addSpacerItem(bar_spacer_2);

  if (slot_right_ == nullptr) {
    slot_right_ = new QHBoxLayout();
    slot_right_->setContentsMargins(0, 0, 0, 0);
    slot_right_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    slot_right_->setSpacing(4);
  }
  bar_layout->addLayout(slot_right_);

  LoadLeftSlot();
  LoadMiddleSlot();
  LoadRightSlot();
  HandleTheme();
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

  if (app_drawer_btn_ == nullptr) {
    LOG(INFO) << absl::StrCat("Now loading application drawer...");
    app_drawer_btn_ = new AppDrawerBtn();
    slot_left_->addWidget(app_drawer_btn_);
  }

  if (krunner_btn_ == nullptr) {
    LOG(INFO) << absl::StrCat("Now loading KRunner toggler...");
    krunner_btn_ = new KRunnerBtn();
    slot_left_->addWidget(krunner_btn_);
  }
}

void MainWindow::LoadMiddleSlot() {
  LOG(INFO) << absl::StrCat("Nothing to load for middle slot...");
}

void MainWindow::LoadRightSlot() {
  if (system_tray_ == nullptr) {
    LOG(INFO) << absl::StrCat("Now loading system tray...");
    system_tray_ = new SystemTray();
    slot_right_->addWidget(system_tray_);
  }

  if (wlan_indicator_ == nullptr) {
    LOG(INFO) << absl::StrCat("Now loading WLAN indicator...");
    wlan_indicator_ = new WLANIndicator();
    slot_right_->addWidget(wlan_indicator_);
  }

  if (battery_indicator_ == nullptr) {
    LOG(INFO) << absl::StrCat("Now loading battery indicator...");
    battery_indicator_ = new BatteryIndicator();
    slot_right_->addWidget(battery_indicator_);
  }

  if (clock_btn_ == nullptr) {
    LOG(INFO) << absl::StrCat("Now loading clock...");
    clock_btn_ = new ClockBtn();
    slot_right_->addWidget(clock_btn_);
  }
}

void MainWindow::TriggerKRunner() {
  backend::ApplicationServices::GetKRunner();
}

}  // namespace frontend
}  // namespace panel
