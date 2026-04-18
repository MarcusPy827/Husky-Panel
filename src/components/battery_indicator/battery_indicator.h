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

#ifndef SRC_COMPONENTS_BATTERY_INDICATOR_BATTERY_INDICATOR_H_
#define SRC_COMPONENTS_BATTERY_INDICATOR_BATTERY_INDICATOR_H_

#include <QObject>
#include <QString>
#include <QVariantList>

#include "src/info_server/battery_info/battery_info.h"

namespace panel {
namespace frontend {

class BatteryIndicator : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString battery_icon
    READ GetBatteryIcon
    NOTIFY BatteryStatusChanged)

  Q_PROPERTY(bool is_charging
    READ GetIsCharging
    NOTIFY BatteryStatusChanged)

  Q_PROPERTY(bool has_builtin_battery
    READ GetHasBuiltinBattery
    NOTIFY BatteryStatusChanged)

  Q_PROPERTY(QVariantList builtin_batteries
    READ GetBuiltinBatteries
    NOTIFY BatteryStatusChanged)

  Q_PROPERTY(QVariantList external_batteries
    READ GetExternalBatteries
    NOTIFY BatteryStatusChanged)

  Q_PROPERTY(QString performance_profile
    READ GetPerformanceProfile
    NOTIFY PerformanceProfileChanged)

  Q_PROPERTY(bool is_sleep_inhibited
    READ GetIsSleepInhibited
    NOTIFY SleepInhibitChanged)

 public:
  explicit BatteryIndicator(QObject* parent = nullptr);
  ~BatteryIndicator() = default;

  QString GetBatteryIcon() const;
  bool GetIsCharging() const;
  bool GetHasBuiltinBattery() const;
  QVariantList GetBuiltinBatteries() const;
  QVariantList GetExternalBatteries() const;
  QString GetPerformanceProfile() const;
  bool GetIsSleepInhibited() const;

  Q_INVOKABLE void setPerformanceProfile(const QString& profile);
  Q_INVOKABLE void setSleepInhibited(bool inhibit);

 signals:
  void BatteryStatusChanged();
  void PerformanceProfileChanged();
  void SleepInhibitChanged();

 private:
  backend::BatteryInfo* battery_info_ = nullptr;
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_BATTERY_INDICATOR_BATTERY_INDICATOR_H_
