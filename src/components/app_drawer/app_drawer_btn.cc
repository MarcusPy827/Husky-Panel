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

#include <QHBoxLayout>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/components/app_drawer/app_drawer_btn.h"

namespace panel {
namespace frontend {

AppDrawerBtn::AppDrawerBtn(QWidget *parent) : QWidget(parent) {
  QHBoxLayout * layout_gen = new QHBoxLayout();
  layout_gen->setContentsMargins(0, 0, 0, 0);
  layout_gen->setSpacing(0);
  setLayout(layout_gen);

  if (btn_ == nullptr) {
    btn_ = new QPushButton();
    btn_->setProperty("class", "common_bar_btn");
  }
  btn_->setText(tr("Applications"));
  layout_gen->addWidget(btn_);
}

AppDrawerBtn::~AppDrawerBtn() {
  LOG(INFO) << absl::StrCat("AppDrawerBtn is being deleted.");
}

QPushButton* AppDrawerBtn::GetBtn() {
  if (btn_ == nullptr) {
    LOG(ERROR) << absl::StrCat("App drawer button is a null pointer, except ",
      "a nullptr passing in!");
  }
  return btn_;
}

}  // namespace frontend
}  // namespace panel
