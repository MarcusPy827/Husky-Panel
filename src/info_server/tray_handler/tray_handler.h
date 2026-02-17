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

#ifndef SRC_INFO_SERVER_TRAY_HANDLER_TRAY_HANDLER_H_
#define SRC_INFO_SERVER_TRAY_HANDLER_TRAY_HANDLER_H_

#include <QObject>
#include <QString>
#include <QDBusServiceWatcher>

#include "src/info_server/tray_handler/tray_def.h"

namespace panel {
namespace backend {

class TrayHandler : public QObject {
  Q_OBJECT

 public:
  explicit TrayHandler();
  ~TrayHandler();
  void InitDataServer();

 private:
  QDBusServiceWatcher * service_watcher_ = nullptr;

 private slots:
  void OnTrayIconRegistered(const QString& service);
  void OnTrayIconUnregistered(const QString& service);

 signals:
  void NewTrayIcon(const QString& services);
  void TrayIconDeleted(const QString& services);
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_TRAY_HANDLER_TRAY_HANDLER_H_
