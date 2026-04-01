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

#include "src/components/battery_indicator/battery_indicator.h"

namespace panel {
namespace frontend {

BatteryIndicator::BatteryIndicator(QObject *parent) : QObject(parent) {
  LOG(INFO) << absl::StrCat("Initializing battery indicator...");
  battery_info_ = new backend::BatteryInfo();
  connect(battery_info_, &backend::BatteryInfo::StatusChanged,
    this, &BatteryIndicator::BatteryStatusChanged);
}

QString BatteryIndicator::GetBatteryIcon() const {
  int level = backend::BatteryInfo::GetBatteryLevel();
  switch (level) {
    case 0 ... 20:  return QStringLiteral("battery_android_alert");
    case 21 ... 30: return QStringLiteral("battery_android_frame_2");
    case 31 ... 40: return QStringLiteral("battery_android_frame_3");
    case 41 ... 50: return QStringLiteral("battery_android_frame_4");
    case 51 ... 75: return QStringLiteral("battery_android_frame_5");
    case 76 ... 90: return QStringLiteral("battery_android_frame_6");
    case 91 ... 100: return QStringLiteral("battery_android_full");
    default:        return QStringLiteral("battery_android_frame_question");
  }
}

bool BatteryIndicator::GetIsCharging() const {
  return backend::BatteryInfo::GetIsCharging();
}

}  // namespace frontend
}  // namespace panel
