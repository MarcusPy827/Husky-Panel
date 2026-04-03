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

#ifndef SRC_CONFIG_LOADER_SYSTEM_TRAY_TRAY_CONFIG_HANDLER_H_
#define SRC_CONFIG_LOADER_SYSTEM_TRAY_TRAY_CONFIG_HANDLER_H_

#include <QObject>
#include <QString>

#include <vector>

#define CONFIG_DIR_NAME "marcus.husky.panel"

namespace panel {
namespace loader {

struct TrayConfig {
  QString id;
  QString friendly_name;
  bool is_visible = true;
};

class TrayConfigHandler : public QObject {
  Q_OBJECT

 public:
  explicit TrayConfigHandler(QObject* parent = nullptr);
  ~TrayConfigHandler();

  std::vector<TrayConfig> GetTrayConfigs();
};

}  // namespace loader
}  // namespace panel

#endif  // SRC_CONFIG_LOADER_SYSTEM_TRAY_TRAY_CONFIG_HANDLER_H_
