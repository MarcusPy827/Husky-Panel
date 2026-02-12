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

#ifndef SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_NULL_IMPL_H_
#define SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_NULL_IMPL_H_

#include "src/info_server/current_window/current_window_provider.h"

namespace panel {
namespace backend {

class CurrentWindowNullImpl : public CurrentWindowProvider{
  Q_OBJECT

 public:
  explicit CurrentWindowNullImpl(QObject *parent = nullptr);
  ~CurrentWindowNullImpl();

  QString GetApplicationName() override;
  QString GetPackageName() override;
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_NULL_IMPL_H_
