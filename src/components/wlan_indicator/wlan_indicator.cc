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

#include "src/components/wlan_indicator/wlan_indicator.h"
#include "src/font_loader/font_loader.h"
#include "src/info_server/wlan_info/wlan_info.h"

namespace panel {
namespace frontend {

WLANIndicator::WLANIndicator(QWidget *parent) : QWidget(parent) {
  QHBoxLayout * layout_gen = new QHBoxLayout();
  layout_gen->setContentsMargins(0, 0, 0, 0);
  layout_gen->setSpacing(0);
  setLayout(layout_gen);

  if (btn_ == nullptr) {
    btn_ = new QPushButton();
    btn_->setProperty("class", "common_bar_small_icon_btn");
  }
  btn_->setText("wifi_find");
  btn_->setFont(loader::FontLoader::GetRoundedMaterialSymbolFont());
  layout_gen->addWidget(btn_);

  QTimer * timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this,
    &WLANIndicator::UpdateWLANStrength);
  timer->start(1000);
}

WLANIndicator::~WLANIndicator() {
  LOG(INFO) << absl::StrCat("Battery indicator is being deleted.");
}

QPushButton* WLANIndicator::GetBtn() {
  if (btn_ == nullptr) {
    LOG(ERROR) << absl::StrCat("Battery indicator button is a null pointer! ",
      "Except a nullptr passing in!");
  }
  return btn_;
}

void WLANIndicator::UpdateWLANStrength() {
  if (btn_ == nullptr) {
    LOG(ERROR) << absl::StrCat("Battery indicator button is a null pointer! ",
      "Skipping updating icons...");
    return;
  }

  int signal_strength = backend::WlanInfo::GetWlanSignalStrength();
  QString wlan_icon_name;
  switch (signal_strength) {
    case -1:
      wlan_icon_name = "signal_wifi_off";
      break;

    case 0 ... 10:
      wlan_icon_name = "signal_wifi_0_bar";
      break;

    case 11 ... 25:
      wlan_icon_name = "network_wifi_1_bar";
      break;

    case 26 ... 50:
      wlan_icon_name = "network_wifi_2_bar";
      break;

    case 51 ... 75:
      wlan_icon_name = "network_wifi";
      break;

    case 76 ... 100:
      wlan_icon_name = "signal_wifi_4_bar";
      break;

    default:
      wlan_icon_name = "wifi_find";
      break;
  }
  btn_->setText(wlan_icon_name);
}

}  // namespace frontend
}  // namespace panel
