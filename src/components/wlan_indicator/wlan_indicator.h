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

#ifndef SRC_COMPONENTS_WLAN_INDICATOR_WLAN_INDICATOR_H_
#define SRC_COMPONENTS_WLAN_INDICATOR_WLAN_INDICATOR_H_

#include <QObject>
#include <QString>

#include "src/info_server/wlan_info/wlan_info.h"

namespace panel {
namespace frontend {

class WLANIndicator : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString wlan_icon
    READ GetWlanIcon
    NOTIFY WlanStatusChanged)

 public:
  explicit WLANIndicator(QObject *parent = nullptr);
  ~WLANIndicator() = default;

  QString GetWlanIcon() const;

 signals:
  void WlanStatusChanged();

 private:
  backend::WlanInfo * wlan_info_ = nullptr;
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_WLAN_INDICATOR_WLAN_INDICATOR_H_
