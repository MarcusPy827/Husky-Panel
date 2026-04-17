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

#ifndef SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_XORG_IMPL_H_
#define SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_XORG_IMPL_H_

#include <xcb/xcb.h>

#include <QSocketNotifier>
#include <QTimer>

#include "src/info_server/current_window/current_window_provider.h"

namespace panel {
namespace backend {

class CurrentWindowXorgImpl : public CurrentWindowProvider {
  Q_OBJECT

 public:
  explicit CurrentWindowXorgImpl(QObject* parent = nullptr);
  ~CurrentWindowXorgImpl() override;

  QString GetApplicationName() override;
  QString GetPackageName() override;

 private:
  xcb_atom_t InternAtom(const char* name);
  void UpdateActiveWindow();
  QString GetWindowClass(xcb_window_t window);

  xcb_connection_t* conn_;
  xcb_window_t root_;
  xcb_atom_t atom_net_active_window_;
  xcb_atom_t atom_wm_class_;

  QSocketNotifier* notifier_;
  QTimer* poll_timer_;
  xcb_window_t last_active_window_;
  CurrentWindowInfo window_info_;

 private slots:
  void OnXcbEvent();
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_XORG_IMPL_H_
