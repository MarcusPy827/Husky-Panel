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

#ifndef SRC_COMPONENTS_APP_INDICATOR_APP_INDICATOR_H_
#define SRC_COMPONENTS_APP_INDICATOR_APP_INDICATOR_H_

#include <memory>
#include <QWidget>
#include <QLabel>

#include "src/info_server/current_window/current_window_provider_factory.h"
#include "src/info_server/current_window/current_window_provider.h"

namespace panel {
namespace frontend {

class AppIndicator : public QWidget {
  Q_OBJECT

 public:
  explicit AppIndicator(QWidget *parent = nullptr);
  ~AppIndicator();

 private:
  QLabel * app_name_ = nullptr;
  QLabel * app_package_name_ = nullptr;
  std::unique_ptr<backend::CurrentWindowProvider>
    current_window_info_ = nullptr;

 private slots:
  void OnCurrentWindowChanged();
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_APP_INDICATOR_APP_INDICATOR_H_
