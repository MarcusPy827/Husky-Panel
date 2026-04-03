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

#include <QStandardPaths>
#include <QDir>

#include "src/config_loader/path_finder/path_finder.h"

namespace panel {
namespace loader {

/**
 * @brief Gets the system configuration directory.
 * 
 * @note This function relies on @c QStandardPaths::writableLocation and
 *       assumes that that directory exists and writable.
 * @return (QString) The system configuration directory.
 */

QString PathFinder::GetSystemConfigDir() {
  return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
    + "/";
}


/**
 * @brief Gets the base directory for configuration files.
 * 
 * @details This function first gets the target path, it will create the
 *          directory if it doesn't exist, and then return the path.
 * @note This function relies on @c QStandardPaths::writableLocation.
 * @see @c GetSystemConfigDir().
 * @return (QString) The base directory for configuration files.
 */

QString PathFinder::GetConfigBaseDir() {
  QString target = GetSystemConfigDir() + CONFIG_DIR_NAME + "/";
  QDir dir_existence_test(target);
  if (!dir_existence_test.exists()) {
    QDir().mkpath(target);
  }

  return target;
}

/**
 * @brief Gets the path for the system tray configuration file.
 * 
 * @note This function relies on @c QStandardPaths::writableLocation.
 * @note This function does NOT do the existence check.
 * @see @c GetConfigBaseDir().
 * @return (QString) The path for the system tray configuration file.
 */

QString PathFinder::GetTrayConfigPath() {
  return GetConfigBaseDir() + "system_tray.conf";
}

}  // namespace loader
}  // namespace panel
