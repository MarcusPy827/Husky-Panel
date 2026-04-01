/*
 * Copyright (C) 2025 MarcusPy827
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

#ifndef SRC_INFO_SERVER_CLOCK_CLOCK_H_
#define SRC_INFO_SERVER_CLOCK_CLOCK_H_

#include <QString>
#include <QObject>
#include <QTimer>

#include "src/translation_loader/translation_loader.h"

namespace panel {
namespace backend {

class Clock : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString clock_text READ GetClockText NOTIFY ClockTextChanged)

 public:
  explicit Clock(QObject * parent = nullptr);
  ~Clock();

  QString GetClockText() const;

 signals:
  void ClockTextChanged();

 private:
  QString GetTranslatedTheDayOfTheWeek(QString in);
  QString GetOptimizedDateString(QString in);
  QTimer * clock_timer_ = nullptr;
  QString clock_text_;
  loader::TranslationLoader * translator_ = nullptr;
  QString Tr(const QString& msg);

 private slots:
  void UpdateTime();
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_CLOCK_CLOCK_H_
