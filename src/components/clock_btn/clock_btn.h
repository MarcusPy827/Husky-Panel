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

#ifndef SRC_COMPONENTS_CLOCK_BTN_CLOCK_BTN_H_
#define SRC_COMPONENTS_CLOCK_BTN_CLOCK_BTN_H_

#include <QString>
#include <QObject>

#include "src/info_server/clock/clock.h"
#include "src/components/calendar/calendar.h"

namespace panel {
namespace frontend {

class ClockBtn : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString clock_text READ GetClockText NOTIFY ClockTextChanged)

 public:
  explicit ClockBtn(Calendar * calendar_in, QObject *parent = nullptr);
  ~ClockBtn() = default;

  QString GetClockText() const;

  /* ---------- For QML usage ---------- */
  Q_INVOKABLE void toggleCalendar();

 signals:
  void ClockTextChanged();

 private:
  backend::Clock * clock_ = nullptr;
  Calendar * calendar_ = nullptr;
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_CLOCK_BTN_CLOCK_BTN_H_
