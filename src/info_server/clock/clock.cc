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

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

#include "src/info_server/clock/clock.h"

namespace panel {
namespace backend {

Clock::Clock(QObject * parent) : QObject(parent) {
  if (translator_ == nullptr) {
    translator_ = new loader::TranslationLoader(
      ":/translations/translations/bar.locale", loader::LanguageType::EN_US);
  }

  LOG(INFO) << absl::StrCat("Installing clock updater...");
  clock_timer_ = new QTimer(this);
  connect(clock_timer_, SIGNAL(timeout()), this, SLOT(UpdateTime()));
  clock_timer_->start(1000);
  UpdateTime();
  LOG(INFO) << absl::StrCat("Clock updater installed successfully!!");
}

Clock::~Clock() {
  LOG(INFO) << absl::StrCat("Disconnecting clock signal/slots...");
  disconnect(clock_timer_, SIGNAL(timeout()), this, SLOT(UpdateTime()));
  LOG(INFO) << absl::StrCat("Clock signal/slots disconnected successfully!!");
}

QString Clock::GetClockText() const {
  return clock_text_;
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

  clock_text_ = current_hrs_str + ":" + current_timer.toString("mm")
    + " " + is_afternoon_desc + "  " + current_timer.toString(Tr("yyyy-M-"))
    + GetOptimizedDateString(current_timer.toString(Tr("dd")))
    + " " + GetTranslatedTheDayOfTheWeek(current_timer.toString("ddd"));
  emit ClockTextChanged();
}

QString Clock::GetTranslatedTheDayOfTheWeek(QString in) {
  static const QMap<QString, QString> translated = {
    {"mon", Tr("Mon")},
    {"tue", Tr("Tue")},
    {"wed", Tr("Wed")},
    {"thu", Tr("Thu")},
    {"fri", Tr("Fri")},
    {"sat", Tr("Sat")},
    {"sun", Tr("Sun")}
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

QString Clock::Tr(const QString& msg) {
  if (translator_ == nullptr) {
    return msg;
  } else {
    return translator_->GetTranslation(msg);
  }
}

}  // namespace backend
}  // namespace panel
