/* Copyright (C) 2026 MarcusPy827
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the Free 
 * Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "src/utils/log/log.h"

namespace flake {
namespace utils {

QString Log::Info(const QString& tag, const QString& content) {
  qInfo().noquote() << LOG_BLUE_BOLD "[INFO] " + tag +  LOG_RESET + ": " +
    content;
  return "[INFO] " + tag + ": " + content;
}

QString Log::Info(const QString& tag, const QString& title,
    const QString& content) {
  qInfo().noquote() << LOG_BLUE_BOLD "[INFO] " + tag + " - " + LOG_UNDERLINE +
    title + LOG_RESET +  ": " + content;
  return "[INFO] " + tag + " - " + title + ": " + content;
}

QString Log::Ok(const QString& tag, const QString& content) {
  qDebug().noquote() << LOG_GREEN_BOLD "[ OK ] " + tag +  LOG_RESET + ": " +
    content;
  return "[ OK ] " + tag + ": " + content;
}

QString Log::Ok(const QString& tag, const QString& title,
    const QString& content) {
  qDebug().noquote() << LOG_GREEN_BOLD "[ OK ] " + tag + " - " + LOG_UNDERLINE +
    title + LOG_RESET +  ": " + content;
  return "[ OK ] " + tag + " - " + title + ": " + content;
}

QString Log::Warn(const QString& tag, const QString& content) {
  qWarning().noquote() << LOG_YELLOW_BOLD "[WARN] " + tag +  LOG_RESET +
    ": " + content;
  return "[WARN] " + tag + ": " + content;
}

QString Log::Warn(const QString& tag, const QString& title,
    const QString& content) {
  qWarning().noquote() << LOG_YELLOW_BOLD "[WARN] " + tag + " - " +
    LOG_UNDERLINE + title + LOG_RESET +  ": " + content;
  return "[WARN] " + tag + " - " + title + ": " + content;
}

QString Log::Error(const QString& tag, const QString& content) {
    qCritical().noquote() << LOG_RED_BOLD "[ERROR] " + tag +  LOG_RESET +
      ": " + content;
    return "[ERROR] " + tag + ": " + content;
}

QString Log::Error(const QString& tag, const QString& title,
    const QString& content) {
  qCritical().noquote() << LOG_RED_BOLD "[ERROR] " + tag + " - " +
    LOG_UNDERLINE + title + LOG_RESET +  ": " + content;
  return "[ERROR] " + tag + " - " + title + ": " + content;
}

}  // namespace utils
}  // namespace flake
