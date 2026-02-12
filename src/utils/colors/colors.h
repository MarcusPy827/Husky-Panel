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

#ifndef SRC_UTILS_COLORS_COLORS_H_
#define SRC_UTILS_COLORS_COLORS_H_

#include <QString>

namespace flake {
namespace utils {
class Colors {
 public:
  static QString ApplyOpacityToHexColor(const QString& hex,
    const double & opacity);
};

}  // namespace utils
}  // namespace flake

#endif  // SRC_UTILS_COLORS_COLORS_H_
