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

#include "src/components/wlan_indicator/wlan_indicator.h"

namespace panel {
namespace frontend {

WLANIndicator::WLANIndicator(QObject *parent) : QObject(parent) {
  LOG(INFO) << absl::StrCat("Initializing WLAN indicator...");
  wlan_info_ = new backend::WlanInfo(this);
  connect(wlan_info_, &backend::WlanInfo::WlanStatusChanged,
    this, &WLANIndicator::WlanStatusChanged);
}

QString WLANIndicator::GetWlanIcon() const {
  int signal_strength = backend::WlanInfo::GetWlanSignalStrength();
  switch (signal_strength) {
    case -1:         return QStringLiteral("signal_wifi_off");
    case 0 ... 10:   return QStringLiteral("signal_wifi_0_bar");
    case 11 ... 25:  return QStringLiteral("network_wifi_1_bar");
    case 26 ... 50:  return QStringLiteral("network_wifi_2_bar");
    case 51 ... 75:  return QStringLiteral("network_wifi");
    case 76 ... 100: return QStringLiteral("signal_wifi_4_bar");
    default:         return QStringLiteral("wifi_find");
  }
}

}  // namespace frontend
}  // namespace panel
