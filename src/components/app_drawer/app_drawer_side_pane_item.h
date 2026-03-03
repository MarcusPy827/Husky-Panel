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

#ifndef SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_SIDE_PANE_ITEM_H_
#define SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_SIDE_PANE_ITEM_H_

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QHBoxLayout>

namespace panel {
namespace frontend {

class AppDrawerSidePaneItem : public QWidget {
  Q_OBJECT

 public:
  explicit AppDrawerSidePaneItem(QString icon, QString name,
    QString group_identifer, QString id, bool is_default = false,
    QWidget * parent = nullptr);
  ~AppDrawerSidePaneItem();

 public slots:
  void OnGroupUpdated(const QString& group_identifier, const QString& id);

 protected:
  void mousePressEvent(QMouseEvent *event) override;

 private:
  QWidget * container_ = nullptr;
  QWidget * container_active_ = nullptr;
  QHBoxLayout * layout_gen_ = nullptr;
  QLabel * icon_ = nullptr;
  QLabel * label_ = nullptr;
  QString id_ = "unknown";
  QString group_identifier_ = "unknown";

 signals:
  void GroupSelected(const QString& group_identifier, const QString& id);
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_SIDE_PANE_ITEM_H_
