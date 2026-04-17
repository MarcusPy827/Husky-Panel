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

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"

#include "src/components/clock_btn/clock_btn.h"

namespace panel {
namespace frontend {

ClockBtn::ClockBtn(Calendar * calendar_in, QObject *parent) : QObject(parent) {
  LOG(INFO) << absl::StrCat("Initializing clock...");
  clock_ = new backend::Clock(this);
  connect(clock_, &backend::Clock::ClockTextChanged,
    this, &ClockBtn::ClockTextChanged);

  LOG(INFO) << absl::StrCat("Initializing calendar...");

  // Depreciated old QWidget-based calendar.
  if (calendar_in != nullptr) {
    calendar_ = calendar_in;
  }
}

QString ClockBtn::GetClockText() const {
  return clock_ ? clock_->GetClockText() : QString();
}

void ClockBtn::toggleCalendar() {
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
