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

#ifndef SRC_COMPONENTS_APP_DRAWER_APP_INFO_WRAPPER_H_
#define SRC_COMPONENTS_APP_DRAWER_APP_INFO_WRAPPER_H_

#include <QObject>
#include <QVariantList>
#include <QString>

namespace panel {
namespace frontend {

class AppInfoWrapper : public QObject {
  Q_OBJECT

  Q_PROPERTY(QVariantList allApps READ GetAllApps NOTIFY appsChanged)

 public:
  /* ---------- For backend usage ---------- */
  explicit AppInfoWrapper(QObject* parent = nullptr);
  QVariantList GetAllApps() const;

  /* ---------- For QML usage ---------- */
  Q_INVOKABLE QVariantList appsByCategory(const QString& category) const;

 signals:
  void appsChanged();

 private slots:
  void Refresh();

 private:
  QVariantList all_apps_;
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_APP_DRAWER_APP_INFO_WRAPPER_H_
