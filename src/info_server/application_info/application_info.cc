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

#include <algorithm>

#include <KService>
#include <KServiceGroup>

#include "src/info_server/application_info/application_info.h"

namespace panel {
namespace backend {

QList<AppInfo> ApplicationInfo::GetAllAppications() {
  QList<AppInfo> result;
  const QList<KService::Ptr> services = KService::allServices();

  for (const auto& cur : services) {
    if (!cur->isApplication()) {
      continue;
    }

    if (cur->noDisplay()) {
      continue;
    }

    result.append(KService2AppInfo(cur));
  }

  std::sort(result.begin(), result.end(), [] (const AppInfo& a,
      const AppInfo& b) {
    return a.name.localeAwareCompare(b.name) < 0;
  });

  return result;
}

}  // namespace backend
}  // namespace panel
