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

#include <QString>
#include <QObject>

namespace panel {
namespace backend {

struct CurrentWindowInfo {
  QString application_name;
  QString package_name;
  uint pid;
};

class CurrentWindowProvider : public QObject {
  Q_OBJECT

 public:
  explicit CurrentWindowProvider(QObject *parent = nullptr) : QObject(
    parent) {}
  virtual ~CurrentWindowProvider() = default;

  virtual QString GetApplicationName() = 0;
  virtual QString GetPackageName() = 0;

 signals:
  void CurrentWindowChanged();
};

}  // namespace backend
}  // namespace panel
