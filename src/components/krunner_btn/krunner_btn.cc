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

#include "src/components/krunner_btn/krunner_btn.h"
#include "src/application_services/application_services.h"

namespace panel {
namespace frontend {

KRunnerBtn::KRunnerBtn(QWidget *parent) : QWidget(parent) {
  QHBoxLayout * layout_gen = new QHBoxLayout();
  layout_gen->setContentsMargins(0, 0, 0, 0);
  layout_gen->setSpacing(0);
  setLayout(layout_gen);

  if (btn_ == nullptr) {
    btn_ = new QPushButton();
    btn_->setProperty("class", "common_bar_btn");
    QObject::connect(btn_, &QPushButton::clicked, this,
      &KRunnerBtn::ToggleKRunner);
  }
  btn_->setText(tr("Search"));
  layout_gen->addWidget(btn_);
}

KRunnerBtn::~KRunnerBtn() {
  LOG(INFO) << absl::StrCat("KRunnerBtn is being deleted.");
}

QPushButton* KRunnerBtn::GetBtn() {
  if (btn_ == nullptr) {
    LOG(ERROR) << absl::StrCat("Search button is a null pointer, except ",
      "a nullptr passing in!");
  }
  return btn_;
}

void KRunnerBtn::ToggleKRunner() {
  backend::ApplicationServices::GetKRunner();
}

}  // namespace frontend
}  // namespace panel
