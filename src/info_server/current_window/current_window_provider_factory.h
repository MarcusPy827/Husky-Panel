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

#pragma once
#ifndef SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_PROVIDER_FACTORY_H_
#define SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_PROVIDER_FACTORY_H_

#include <memory>

namespace panel {
namespace backend {

class CurrentWindowProvider;
std::unique_ptr<CurrentWindowProvider>InitCurrentWindowInfoServer(
  QObject *parent);

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_PROVIDER_FACTORY_H_
