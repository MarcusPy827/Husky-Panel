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

#ifndef SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_KWIN_IMPL_H_
#define SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_KWIN_IMPL_H_

#include "src/info_server/current_window/current_window_provider.h"

namespace panel {
namespace backend {

class CurrentWindowKwinImpl : public CurrentWindowProvider{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "marcus.panel.util.AppBridge")

 public:
  explicit CurrentWindowKwinImpl(QObject *parent = nullptr);
  ~CurrentWindowKwinImpl();

  QString GetApplicationName() override;
  QString GetPackageName() override;

 private:
  CurrentWindowInfo window_info_;

 public slots:
  Q_INVOKABLE void UpdateActiveApp(QString name, QString appid, QString title);

 private slots:
  void OnActiveWindowChanged();
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_KWIN_IMPL_H_
