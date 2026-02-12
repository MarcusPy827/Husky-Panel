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

#ifndef SRC_UTILS_STRINGS_STRINGS_H_
#define SRC_UTILS_STRINGS_STRINGS_H_

#include <QString>

namespace flake {
namespace utils {
class Strings {
 public:
  static QString TemplateCat(QString original,
    QList<QString> args = QList<QString>());
};

}  // namespace utils
}  // namespace flake

#endif  // SRC_UTILS_STRINGS_STRINGS_H_
