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

#ifndef SRC_WLAN_INFO_WLAN_INFO_H_
#define SRC_WLAN_INFO_WLAN_INFO_H_
#define WI_WIFI_TYPE 2

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

namespace panel {
namespace backend {

class WlanInfo : public QObject {
  Q_OBJECT

 public:
  explicit WlanInfo(QObject* parent = nullptr);
  ~WlanInfo() = default;

  static int GetWlanSignalStrength();

 signals:
  void WlanStatusChanged();

 private:
  QString wifi_device_path_;
  QString active_ap_path_;

  void FindAndSubscribeToWifiDevice();
  void SubscribeToAp(const QString& ap_path);
  void UnsubscribeFromAp(const QString& ap_path);

 private slots:
  void OnDevicePropertiesChanged(const QString& interface,
    const QVariantMap& changed, const QStringList& invalidated);
  void OnApPropertiesChanged(const QString& interface,
    const QVariantMap& changed, const QStringList& invalidated);
  void OnNmStateChanged(uint state);
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_WLAN_INFO_WLAN_INFO_H_
