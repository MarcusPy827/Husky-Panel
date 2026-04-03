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

#ifndef SRC_CONFIG_LOADER_PATH_FINDER_PATH_FINDER_H_
#define SRC_CONFIG_LOADER_PATH_FINDER_PATH_FINDER_H_

#include <QString>

#define CONFIG_DIR_NAME "marcus.husky.panel"

namespace panel {
namespace loader {

class PathFinder {
 public:
  /* ---------- Directories ---------- */
  static QString GetSystemConfigDir();
  static QString GetConfigBaseDir();

  /* ---------- Files ---------- */
  static QString GetTrayConfigPath();
};

}  // namespace loader
}  // namespace panel

#endif  // SRC_CONFIG_LOADER_PATH_FINDER_PATH_FINDER_H_
