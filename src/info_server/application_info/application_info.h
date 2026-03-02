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

#ifndef SRC_INFO_SERVER_APPLICATION_INFO_APPLICATION_INFO_H_
#define SRC_INFO_SERVER_APPLICATION_INFO_APPLICATION_INFO_H_

#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <KService>

struct AppInfo {
  QString id;
  QString name;
  QString generic_name;
  QString comment;
  QString icon_name;
  QString exec;
  QStringList categories;
};

enum class AppCategory {
  kAll = 0,
  kAudioVideo = 1,
  kDevelopment = 2,
  kEducation = 3,
  kGame = 4,
  kGraphics = 5,
  kNetwork = 6,
  kOffice = 7,
  kSettings = 8,
  kSystem = 9,
  kUtilities = 10
};

namespace panel {
namespace backend {

class ApplicationInfo {
 public:
  static QList<AppInfo> GetAllAppications();

 private:
  static AppInfo KService2AppInfo(const KService::Ptr &service);
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_APPLICATION_INFO_APPLICATION_INFO_H_
