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
#include <QTimer>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/components/battery_indicator/battery_indicator.h"
#include "src/font_loader/font_loader.h"
#include "src/info_server/battery_info/battery_info.h"

namespace panel {
namespace frontend {

BatteryIndicator::BatteryIndicator(QWidget *parent) : QWidget(parent) {
  QHBoxLayout * layout_gen = new QHBoxLayout();
  layout_gen->setContentsMargins(0, 0, 0, 0);
  layout_gen->setSpacing(0);
  setLayout(layout_gen);

  if (btn_ == nullptr) {
    btn_ = new QPushButton();
    btn_->setProperty("class", "common_bar_icon_btn");
  }
  btn_->setText("battery_android_frame_question");
  btn_->setFont(loader::FontLoader::GetRoundedMaterialSymbolFont());
  layout_gen->addWidget(btn_);

  QTimer * timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this,
    &BatteryIndicator::UpdateBatteryPercentage);
  timer->start(1000);
}

BatteryIndicator::~BatteryIndicator() {
  LOG(INFO) << absl::StrCat("Battery indicator is being deleted.");
}

QPushButton* BatteryIndicator::GetBtn() {
  if (btn_ == nullptr) {
    LOG(ERROR) << absl::StrCat("Battery indicator button is a null pointer! ",
      "Except a nullptr passing in!");
  }
  return btn_;
}

void BatteryIndicator::UpdateBatteryPercentage() {
  if (btn_ == nullptr) {
    LOG(ERROR) << absl::StrCat("Battery indicator button is a null pointer! ",
      "Skipping updating icons...");
    return;
  }

  int battery_level = backend::BatteryInfo::GetBatteryLevel();
  QString battery_icon_name = "battery_android_frame_";
  switch (battery_level) {
    case 0 ... 20:
      battery_icon_name = "battery_android_alert";
      break;

    case 21 ... 30:
      battery_icon_name += "2";
      break;

    case 31 ... 40:
      battery_icon_name += "3";
      break;

    case 41 ... 50:
      battery_icon_name += "4";
      break;

    case 51 ... 75:
      battery_icon_name += "5";
      break;

    case 76 ... 90:
      battery_icon_name += "6";
      break;

    case 91 ... 100:
      battery_icon_name = "battery_android_full";
      break;

    default:
      battery_icon_name = "battery_android_frame_question";
      break;
  }
  btn_->setText(battery_icon_name);
}

}  // namespace frontend
}  // namespace panel
