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

#include "src/components/clock_btn/clock_btn.h"

namespace panel {
namespace frontend {

ClockBtn::ClockBtn(Calendar * calandar_in, QWidget *parent) : QWidget(parent) {
  QHBoxLayout * layout_gen = new QHBoxLayout();
  layout_gen->setContentsMargins(0, 0, 0, 0);
  layout_gen->setSpacing(0);
  setLayout(layout_gen);

  if (translator_ == nullptr) {
    translator_ = new loader::TranslationLoader(
      ":/translations/translations/bar.locale", loader::LanguageType::EN_US);
  }

  if (btn_ == nullptr) {
    btn_ = new QPushButton();
    btn_->setProperty("class", "common_bar_btn");
  }
  
  if (translator_ != nullptr) {
    btn_->setText(translator_->GetTranslation("INITIALIZING CLOCK..."));
  } else {
    btn_->setText("INITIALIZING CLOCK...");
  }

  QObject::connect(btn_, &QPushButton::clicked, this,
    &ClockBtn::ToggleCalendar);
  layout_gen->addWidget(btn_);

  LOG(INFO) << absl::StrCat("Initializing clock...");
  if (clock_updater_ == nullptr) {
    clock_updater_ = new backend::Clock(btn_);
  }

  LOG(INFO) << absl::StrCat("Initializing calendar...");
  if (calandar_in == nullptr) {
    LOG(ERROR) << absl::StrCat("Calendar widget is null pointer, calendar ",
      "widget will NOT show.");
  } else {
    calendar_ = calandar_in;
  }
}

ClockBtn::~ClockBtn() {
  LOG(INFO) << absl::StrCat("ClockBtn is being deleted.");
}

QPushButton* ClockBtn::GetBtn() {
  if (btn_ == nullptr) {
    LOG(ERROR) << absl::StrCat("Clock button is a null pointer, except a ",
      "nullptr passing in!");
  }
  return btn_;
}

void ClockBtn::ToggleCalendar() {
  if (calendar_ == nullptr) {
    LOG(ERROR) << absl::StrCat(
      "Calendar widget is NOT initialized, aborting...");
    return;
  }

  if (calendar_->isVisible()) {
    calendar_->hide();
  } else {
    calendar_->show();
  }
}

}  // namespace frontend
}  // namespace panel
