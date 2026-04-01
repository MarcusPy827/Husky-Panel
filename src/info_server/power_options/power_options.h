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

#ifndef SRC_INFO_SERVER_POWER_OPTIONS_POWER_OPTIONS_H_
#define SRC_INFO_SERVER_POWER_OPTIONS_POWER_OPTIONS_H_

#include <QObject>

namespace panel {
namespace backend {

class PowerOptions : public QObject {
  Q_OBJECT

 public:
  explicit PowerOptions(QObject* parent = nullptr);
  ~PowerOptions() override = default;

  /* ---------- For backend useage ---------- */
  void Suspend();
  void Hibernate();
  void Reboot();
  void PowerOff();

  /* ---------- For QML useage ---------- */
  Q_INVOKABLE void suspend();
  Q_INVOKABLE void hibernate();
  Q_INVOKABLE void reboot();
  Q_INVOKABLE void powerOff();

 private:
  void CallLogind(const QString& method);
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_POWER_OPTIONS_POWER_OPTIONS_H_
