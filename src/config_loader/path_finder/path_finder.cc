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

#include <sysexits.h>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

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
 * @note The program crashes if the config directory failed to create.
 * @see @c GetSystemConfigDir().
 * @return (QString) The base directory for configuration files.
 */

QString PathFinder::GetConfigBaseDir() {
  QString target = GetSystemConfigDir() + CONFIG_DIR_NAME + "/";
  QDir dir_existence_test(target);
  if (!dir_existence_test.exists()) {
    if (!QDir().mkpath(target)) {
      LOG(ERROR) << absl::StrCat("Failed to create config directory!!");
      exit(EX_IOERR);
    }
  }

  return target;
}

/**
 * @brief Gets the path for the system tray configuration file.
 * 
 * @note This function relies on @c QStandardPaths::writableLocation.
 * @note An empty file will be created if the target file does NOT exist.
 * @note The program crashes if the config directory failed to create.
 * @see @c GetConfigBaseDir().
 * @return (QString) The path for the system tray configuration file.
 */

QString PathFinder::GetTrayConfigPath() {
  QString target = GetConfigBaseDir() + "system_tray.conf";
  QFile file(target);
  if (!file.exists()) {
    if (file.open(QIODevice::WriteOnly)) {
      file.close();
    } else {
      LOG(ERROR) << absl::StrCat("Failed to create system tray config file!!");
      exit(EX_IOERR);
    }
  }

  return target;
}


/**
 * @brief Gets the path for the system tray expanding icon configuration file.
 * 
 * @note This function relies on @c QStandardPaths::writableLocation.
 * @note An empty file will be created if the target file does NOT exist.
 * @note The program crashes if the config directory failed to create.
 * @see @c GetConfigBaseDir().
 * @return (QString) The path for the system tray expanding icon configuration
 *                   file.
 */

QString PathFinder::GetTrayExpandingIconConfigPath() {
  QString target = GetConfigBaseDir() + "tray_expanding_icon.conf";
  QFile file(target);
  if (!file.exists()) {
    if (file.open(QIODevice::WriteOnly)) {
      file.close();
    } else {
      LOG(ERROR) << absl::StrCat("Failed to create system tray expanding ",
        "icon config file!!");
      exit(EX_IOERR);
    }
  }

  return target;
}

}  // namespace loader
}  // namespace panel
