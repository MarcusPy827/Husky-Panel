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

#ifndef SRC_COMPONENTS_CALENDAR_CALENDAR_H_
#define SRC_COMPONENTS_CALENDAR_CALENDAR_H_

#include <QWidget>
#include <QCalendarWidget>

#include "src/info_server/clock/clock.h"
#include "src/theme_loader/theme_loader.h"

namespace panel {
namespace frontend {

class Calendar : public QWidget {
  Q_OBJECT

 public:
  explicit Calendar(QWidget *parent = nullptr);
  ~Calendar();

 private:
  QCalendarWidget * cal_ = nullptr;
  loader::ThemeLoader * theme_loader_ = nullptr;
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_CALENDAR_CALENDAR_H_
