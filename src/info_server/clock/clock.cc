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

#include <QDebug>
#include <QTimer>
#include <QDateTime>

#include "src/info_server/clock/clock.h"

namespace panel {
namespace backend {

Clock::Clock(QPushButton * target) {
  if (target == nullptr) {
    qCritical() << "[ERROR] Clock: Target pushbutton is null pointer, could"
      << "not install clock updater, aborting...";
    return;
  }

  qInfo() << "[INFO] Clock: Installing clock updater...";
  install_target_ = target;
  clock_timer_ = new QTimer;
  connect(clock_timer_, SIGNAL(timeout()), this, SLOT(UpdateTime()));
  clock_timer_->start(1000);
  qInfo() << "[ OK ] Clock: Clock updater installed successfully!!";
}

Clock::~Clock() {
  qInfo() << "[INFO] Clock: Disconnecting clock signal/slots...";
  disconnect(clock_timer_, SIGNAL(timeout()), this, SLOT(UpdateTime()));
  qInfo() << "[ OK ] Clock: Clock signal/slots disconnected successfully!!";
}

void Clock::UpdateTime() {
  QDateTime current_timer = QDateTime::currentDateTime();

  bool is_afternoon;
  QString current_hrs_str = current_timer.toString("hh");
  int current_hrs_int = current_hrs_str.toInt();

  if (current_hrs_int > 12) {
    current_hrs_int = current_hrs_int - 12;
    is_afternoon = true;
  } else {
    is_afternoon = false;
  }

  bool digit_filling;
  if (current_hrs_int < 10) {
    digit_filling = true;
  } else {
    digit_filling = false;
  }

  current_hrs_str = QString::number(current_hrs_int);
  if (digit_filling) {
    current_hrs_str = "0" + current_hrs_str;
  }

  QString is_afternoon_desc;
  if (is_afternoon) {
    is_afternoon_desc = "P.M.";
  } else {
    is_afternoon_desc = "A.M.";
  }

  QString time_str_gen = current_hrs_str + ":" + current_timer.toString("mm")
    + " " + is_afternoon_desc + "  " + current_timer.toString(tr("yyyy-M-"))
    + GetOptimizedDateString(current_timer.toString(tr("dd")))
    + " " + GetTranslatedTheDayOfTheWeek(current_timer.toString("ddd"));
  if (install_target_ != nullptr) {
    install_target_->setText(time_str_gen);
  }
}

QString Clock::GetTranslatedTheDayOfTheWeek(QString in) {
  static const QMap<QString, QString> translated = {
    {"mon", tr("Mon.")},
    {"tue", tr("Tue.")},
    {"wed", tr("Wed.")},
    {"thu", tr("Thu.")},
    {"fri", tr("Fri.")},
    {"sat", tr("Sat.")},
    {"sun", tr("Sun.")}
  };
  return translated.value(in.toLower(), in);
}

QString Clock::GetOptimizedDateString(QString in) {
  QString result = in;
  if (result.startsWith('0')) {
    result = result.mid(1);
  }
  return result;
}

}  // namespace backend
}  // namespace panel
