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

#include <QVariantMap>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

#include "src/info_server/application_info/application_info.h"
#include "src/components/app_drawer/app_info_wrapper.h"

namespace panel {
namespace frontend {

namespace {
static QVariantMap AppInfoToMap(const AppInfo& app) {
  QVariantMap result;
  result["id"]           = app.id;
  result["name"]         = app.name;
  result["generic_name"] = app.generic_name;
  result["comment"]      = app.comment;
  result["icon_name"]    = app.icon_name;
  result["exec"]         = app.exec;
  result["categories"]   = app.categories;
  return result;
}
}  // namespace

AppInfoWrapper::AppInfoWrapper(QObject* parent) : QObject(parent) {
  const QList<AppInfo> apps = backend::ApplicationInfo::GetAllAppications();
  all_apps_.reserve(apps.size());
  for (const auto& app : apps) {
    all_apps_.append(AppInfoToMap(app));
  }
}

QVariantList AppInfoWrapper::GetAllApps() const {
  return all_apps_;
}

QVariantList AppInfoWrapper::appsByCategory(const QString& category) const {
  QVariantList result;
  for (const auto& entry : all_apps_) {
    const QVariantMap app = entry.toMap();
    if (app["categories"].toStringList().contains(category)) {
      result.append(app);
    }
  }
  return result;
}

}  // namespace frontend
}  // namespace panel
