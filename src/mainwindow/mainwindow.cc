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

#include "src/mainwindow/mainwindow.h"
#include "src/mainwindow/ui_mainwindow.h"
#include "src/user_info/user_info.h"
#include "lib/3rdparty/layer-shell-qt/src/interfaces/window.h"

namespace panel {
namespace frontend {

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new
    Ui::MainWindow) {
  ui->setupUi(this);
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
  ui->user_name_btn->setText(backend::UserInfo::GetUserName());
  ui->avatar_btn->setIcon(
    QIcon(backend::UserInfo::GetUserAvatarPath()));

  qInfo() << "[INFO] Top Bar: Initialization complete.";
}

MainWindow::~MainWindow() {
  delete ui;
}

}  // namespace frontend
}  // namespace panel
