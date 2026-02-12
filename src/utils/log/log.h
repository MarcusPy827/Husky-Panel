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

#ifndef SRC_UTILS_LOG_LOG_H_
#define SRC_UTILS_LOG_LOG_H_

#define LOG_BLUE_BOLD "\033[34;1m"
#define LOG_GREEN_BOLD "\x1b[32;1m"
#define LOG_YELLOW_BOLD "\x1b[33;1m"
#define LOG_RED_BOLD "\x1b[31;1m"
#define LOG_UNDERLINE "\x1b[39;4m"
#define LOG_RESET "\033[0m"

#include <QString>
#include <QDebug>

namespace flake {
namespace utils {
class Log {
 public:
  static QString Info(const QString& tag, const QString& content);
  static QString Info(const QString& tag, const QString& title,
    const QString& content);

  static QString Ok(const QString& tag, const QString& content);
  static QString Ok(const QString& tag, const QString& title,
    const QString& content);

  static QString Warn(const QString& tag, const QString& content);
  static QString Warn(const QString& tag, const QString& title,
    const QString& content);

  static QString Error(const QString& tag, const QString& content);
  static QString Error(const QString& tag, const QString& title,
    const QString& content);
};

}  // namespace utils
}  // namespace flake

#endif  // SRC_UTILS_LOG_LOG_H_
