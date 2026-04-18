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

#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDBusUnixFileDescriptor>
#include <QDebug>
#include <QList>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

#include "src/info_server/battery_info/battery_info.h"
#include "src/utils/dbus_def.h"

namespace panel {
namespace backend {

BatteryInfo::BatteryInfo(QObject* parent) : QObject(parent) {
  QDBusConnection::systemBus().connect(
    DBUS_UPOWER_SERVICE, DBUS_UPOWER_DISPLAY_SERVICE_PATH,
    DBUS_PROPERTIES_INTERFACE, DBUS_PROP_UPDTE_METHOD, this,
    SLOT(OnUpdateProperties(QString, QVariantMap, QStringList)));

  QDBusConnection::systemBus().connect(
    DBUS_POWER_PROFILES_SERVICE, DBUS_POWER_PROFILES_PATH,
    DBUS_PROPERTIES_INTERFACE, DBUS_PROP_UPDTE_METHOD, this,
    SLOT(OnPowerProfileChanged(QString, QVariantMap, QStringList)));
}

int BatteryInfo::GetBatteryLevel() {
  QDBusInterface interface(DBUS_UPOWER_SERVICE,
    DBUS_UPOWER_DISPLAY_SERVICE_PATH, DBUS_UPOWER_DEVICE_INTERFACE,
    QDBusConnection::systemBus());
  if (!interface.isValid()) {
    LOG(ERROR) << absl::StrCat("Failed to connect to UPower D-Bus interface.");
    return 0;
  }
  return interface.property("Percentage").toInt();
}

bool BatteryInfo::GetIsCharging() {
  QDBusInterface interface(DBUS_UPOWER_SERVICE,
    DBUS_UPOWER_DISPLAY_SERVICE_PATH, DBUS_UPOWER_DEVICE_INTERFACE,
    QDBusConnection::systemBus());
  if (!interface.isValid()) {
    LOG(ERROR) << absl::StrCat("Failed to connect to UPower D-Bus interface.");
    return false;
  }
  int state = interface.property("State").toInt();
  return (state == 1 || state == 5);
}

bool BatteryInfo::HasBuiltinBattery() {
  QDBusInterface upower(DBUS_UPOWER_SERVICE, DBUS_UPOWER_PATH,
    DBUS_UPOWER_INTERFACE, QDBusConnection::systemBus());
  if (!upower.isValid()) return false;

  QDBusReply<QList<QDBusObjectPath>> reply = upower.call("EnumerateDevices");
  if (!reply.isValid()) return false;

  for (const QDBusObjectPath& path : reply.value()) {
    QDBusInterface dev(DBUS_UPOWER_SERVICE, path.path(),
      DBUS_UPOWER_DEVICE_INTERFACE, QDBusConnection::systemBus());
    if (!dev.isValid()) continue;
    if (dev.property("Type").toInt() == 2 &&
        dev.property("PowerSupply").toBool()) {
      return true;
    }
  }
  return false;
}

QVariantList BatteryInfo::GetBuiltinBatteries() {
  QVariantList result;
  QDBusInterface upower(DBUS_UPOWER_SERVICE, DBUS_UPOWER_PATH,
    DBUS_UPOWER_INTERFACE, QDBusConnection::systemBus());
  if (!upower.isValid()) return result;

  QDBusReply<QList<QDBusObjectPath>> reply = upower.call("EnumerateDevices");
  if (!reply.isValid()) return result;

  for (const QDBusObjectPath& path : reply.value()) {
    QDBusInterface dev(DBUS_UPOWER_SERVICE, path.path(),
      DBUS_UPOWER_DEVICE_INTERFACE, QDBusConnection::systemBus());
    if (!dev.isValid()) continue;
    if (dev.property("Type").toInt() != 2 ||
        !dev.property("PowerSupply").toBool()) {
      continue;
    }

    QVariantMap entry;

    QString model = dev.property("Model").toString().trimmed();
    entry["name"] = model.isEmpty()
      ? QStringLiteral("Built-in Battery") : model;

    entry["percentage"] = dev.property("Percentage").toInt();

    int state = dev.property("State").toInt();
    bool charging = (state == 1 || state == 5);
    entry["is_charging"] = charging;

    if (state == 5) {
      entry["time_remaining"] = QStringLiteral("Full");
    } else if (charging) {
      qint64 ttf = dev.property("TimeToFull").toLongLong();
      entry["time_remaining"] = (ttf > 0)
        ? FormatTimeRemaining(ttf) : QStringLiteral("Charging");
    } else {
      qint64 tte = dev.property("TimeToEmpty").toLongLong();
      entry["time_remaining"] = (tte > 0)
        ? FormatTimeRemaining(tte) : QStringLiteral("Unknown");
    }

    double capacity = dev.property("Capacity").toDouble();
    entry["health"] = (capacity > 0.0)
      ? QString("%1%").arg(qRound(capacity))
      : QStringLiteral("Unknown");

    result.append(entry);
  }
  return result;
}

QVariantList BatteryInfo::GetExternalBatteries() {
  QVariantList result;
  QDBusInterface upower(DBUS_UPOWER_SERVICE, DBUS_UPOWER_PATH,
    DBUS_UPOWER_INTERFACE, QDBusConnection::systemBus());
  if (!upower.isValid()) return result;

  QDBusReply<QList<QDBusObjectPath>> reply = upower.call("EnumerateDevices");
  if (!reply.isValid()) return result;

  for (const QDBusObjectPath& path : reply.value()) {
    if (path.path() == QLatin1String(DBUS_UPOWER_DISPLAY_SERVICE_PATH)) {
      continue;
    }

    QDBusInterface dev(DBUS_UPOWER_SERVICE, path.path(),
      DBUS_UPOWER_DEVICE_INTERFACE, QDBusConnection::systemBus());
    if (!dev.isValid()) continue;

    int type = dev.property("Type").toInt();
    bool power_supply = dev.property("PowerSupply").toBool();
    if (type == 1) continue;
    if (type == 2 && power_supply) continue;

    double percentage = dev.property("Percentage").toDouble();
    if (percentage <= 0.0) continue;

    QVariantMap entry;
    QString model = dev.property("Model").toString().trimmed();
    if (model.isEmpty()) {
      model = dev.property("NativePath").toString().trimmed();
    }
    entry["name"] = model.isEmpty() ? QStringLiteral("Unknown Device") : model;
    int pct = qRound(percentage);
    entry["percentage"] = pct;
    entry["icon"] = BatteryLevelToIcon(pct);

    result.append(entry);
  }
  return result;
}

QString BatteryInfo::GetPerformanceProfile() {
  QDBusInterface iface(DBUS_POWER_PROFILES_SERVICE, DBUS_POWER_PROFILES_PATH,
    DBUS_POWER_PROFILES_INTERFACE, QDBusConnection::systemBus());
  if (!iface.isValid()) return QStringLiteral("Balanced");

  QString profile = iface.property("ActiveProfile").toString();
  if (profile == QLatin1String("power-saver")) return QStringLiteral("ECO");
  if (profile == QLatin1String("performance"))
    return QStringLiteral("Performance");
  return QStringLiteral("Balanced");
}

bool BatteryInfo::GetIsSleepInhibited() const {
  return is_sleep_inhibited_;
}

void BatteryInfo::SetPerformanceProfile(const QString& profile) {
  QString dbus_profile;
  if (profile == QLatin1String("ECO")) {
    dbus_profile = QStringLiteral("power-saver");
  } else if (profile == QLatin1String("Performance")) {
    dbus_profile = QStringLiteral("performance");
  } else {
    dbus_profile = QStringLiteral("balanced");
  }

  QDBusInterface iface(DBUS_POWER_PROFILES_SERVICE, DBUS_POWER_PROFILES_PATH,
    DBUS_POWER_PROFILES_INTERFACE, QDBusConnection::systemBus());
  if (!iface.isValid()) {
    LOG(ERROR) << absl::StrCat("Failed to connect to power-profiles-daemon.");
    return;
  }
  iface.setProperty("ActiveProfile", dbus_profile);
}

void BatteryInfo::SetSleepInhibited(bool inhibit) {
  if (inhibit == is_sleep_inhibited_) return;

  if (inhibit) {
    QDBusInterface logind(DBUS_LOGIND_SERVICE, DBUS_LOGIND_PATH,
      DBUS_LOGIND_MANAGER_INTERFACE, QDBusConnection::systemBus());
    if (!logind.isValid()) {
      LOG(ERROR) << absl::StrCat("Failed to connect to logind for inhibitor.");
      return;
    }
    QDBusReply<QDBusUnixFileDescriptor> reply = logind.call(
      "Inhibit",
      QStringLiteral("idle:sleep:handle-lid-switch"),
      QStringLiteral("HuskyPanel"),
      QStringLiteral("User requested"),
      QStringLiteral("block"));
    if (!reply.isValid()) {
      LOG(ERROR) << absl::StrCat("Failed to acquire sleep inhibitor: ",
        reply.error().message().toStdString());
      return;
    }
    inhibitor_fd_ = reply.value();
    is_sleep_inhibited_ = true;
  } else {
    inhibitor_fd_ = QDBusUnixFileDescriptor();
    is_sleep_inhibited_ = false;
  }
  emit SleepInhibitChanged();
}

void BatteryInfo::OnUpdateProperties(const QString& interface,
    const QVariantMap& changed, const QStringList& invalidated) {
  Q_UNUSED(changed);
  Q_UNUSED(invalidated);
  if (interface != QLatin1String(DBUS_UPOWER_DEVICE_INTERFACE)) return;
  emit StatusChanged();
}

void BatteryInfo::OnPowerProfileChanged(const QString& interface,
    const QVariantMap& changed, const QStringList& invalidated) {
  Q_UNUSED(changed);
  Q_UNUSED(invalidated);
  if (interface != QLatin1String(DBUS_POWER_PROFILES_INTERFACE)) return;
  emit PerformanceProfileChanged();
}

QString BatteryInfo::FormatTimeRemaining(qint64 seconds) {
  if (seconds <= 0) return QStringLiteral("Unknown");
  qint64 hours = seconds / 3600;
  qint64 minutes = (seconds % 3600) / 60;
  if (hours > 0) {
    return QString("%1h %2m").arg(hours).arg(minutes, 2, 10, QChar('0'));
  }
  return QString("%1m").arg(minutes);
}

QString BatteryInfo::BatteryLevelToIcon(int level) {
  switch (level) {
    case 0 ... 20: return QStringLiteral("battery_android_alert");
    case 21 ... 30: return QStringLiteral("battery_android_frame_2");
    case 31 ... 40: return QStringLiteral("battery_android_frame_3");
    case 41 ... 50: return QStringLiteral("battery_android_frame_4");
    case 51 ... 75: return QStringLiteral("battery_android_frame_5");
    case 76 ... 90: return QStringLiteral("battery_android_frame_6");
    case 91 ... 100: return QStringLiteral("battery_android_full");
    default: return QStringLiteral("battery_android_frame_question");
  }
}

}  // namespace backend
}  // namespace panel
