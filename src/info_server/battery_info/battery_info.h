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

#ifndef SRC_INFO_SERVER_BATTERY_INFO_BATTERY_INFO_H_
#define SRC_INFO_SERVER_BATTERY_INFO_BATTERY_INFO_H_

#include <QDBusUnixFileDescriptor>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

namespace panel {
namespace backend {

class BatteryInfo : public QObject {
  Q_OBJECT
 public:
  explicit BatteryInfo(QObject* parent = nullptr);
  ~BatteryInfo() = default;

  static int GetBatteryLevel();
  static bool GetIsCharging();

  static bool HasBuiltinBattery();
  static QVariantList GetBuiltinBatteries();
  static QVariantList GetExternalBatteries();
  static QString GetPerformanceProfile();

  bool GetIsSleepInhibited() const;
  void SetPerformanceProfile(const QString& profile);
  void SetSleepInhibited(bool inhibit);

 private slots:
  void OnUpdateProperties(const QString& interface, const QVariantMap& changed,
    const QStringList& invalidated);
  void OnPowerProfileChanged(const QString& interface,
    const QVariantMap& changed, const QStringList& invalidated);

 signals:
  void StatusChanged();
  void PerformanceProfileChanged();
  void SleepInhibitChanged();

 private:
  static QString FormatTimeRemaining(qint64 seconds);
  static QString BatteryLevelToIcon(int level);

  bool is_sleep_inhibited_ = false;
  QDBusUnixFileDescriptor inhibitor_fd_;
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_BATTERY_INFO_BATTERY_INFO_H_
