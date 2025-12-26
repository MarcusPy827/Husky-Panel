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

#include <QGraphicsDropShadowEffect>
#include <QProcess>

#include "src/quick_kde_su/quick_kde_su.h"
#include "src/quick_kde_su/ui_quick_kde_su.h"

namespace panel {
namespace frontend {

QuickKDESU::QuickKDESU(QWidget *parent) : QWidget(parent), ui_(new
    Ui::QuickKDESU) {
  ui_->setupUi(this);
  this->setAttribute(Qt::WA_TranslucentBackground);

  QGraphicsDropShadowEffect * menu_shadow = new QGraphicsDropShadowEffect();
  menu_shadow->setBlurRadius(18);
  menu_shadow->setColor(QColor(76, 79, 105, 200));
  menu_shadow->setOffset(0, 0);
  ui_->kdesu_panel_frame->setGraphicsEffect(menu_shadow);

  connect(ui_->kdesu_dolphin_btn, &QToolButton::clicked, this,
    &QuickKDESU::OpenDoplhinWithKDESU);
  connect(ui_->kdesu_konsole_btn, &QToolButton::clicked, this,
    &QuickKDESU::OpenKonsoleWithKDESU);
}

QuickKDESU::~QuickKDESU() {
  delete ui_;
}

void QuickKDESU::ExecWithKDESU(const QString &prog,
    const QStringList &args) {
  QStringList args_gen;
  args_gen << prog << args;
  QProcess::startDetached("kdesu", args_gen);
}

void QuickKDESU::OpenDoplhinWithKDESU() {
  ExecWithKDESU("dolphin", {"~"});
  hide();
}

void QuickKDESU::OpenKonsoleWithKDESU() {
  ExecWithKDESU("konsole", {});
  hide();
}

}  // namespace frontend
}  // namespace panel
