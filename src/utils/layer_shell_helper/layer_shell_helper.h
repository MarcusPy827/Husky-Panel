/*
 * Copyright (C) 2025 MarcusPy827
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

#include <QQuickImageProvider>
#include <QObject>
#include <QIcon>
#include <QPixmap>
#include <QSize>

#ifndef SRC_UTILS_LAYER_SHELL_HELPER_LAYER_SHELL_HELPER_H_
#define SRC_UTILS_LAYER_SHELL_HELPER_LAYER_SHELL_HELPER_H_

namespace panel {
namespace utils {

class LayerShellHelper : public QObject {
  Q_OBJECT

 public:
  explicit LayerShellHelper(QObject* parent = nullptr);

  /* ---------- For backend usage ---------- */
  void SetBarOnlyMask(QObject* window_obj, int bar_height);
  void SetBarAndDrawerMask(QObject* window_obj, int bar_height, int drawer_x,
    int drawer_w, int drawer_h);
  void SetFullMask(QObject* window_obj);
  void SetupOverlayWindow(QObject* window_obj);

  /* ---------- For QML usage ---------- */
  Q_INVOKABLE void setBarOnlyMask(QObject* window_obj, int bar_height);
  Q_INVOKABLE void setBarAndDrawerMask(QObject* window_obj, int bar_height,
    int drawer_x, int drawer_w, int drawer_h);
  Q_INVOKABLE void setFullMask(QObject* window_obj);
  Q_INVOKABLE void setupOverlayWindow(QObject* window_obj);
};

}  // namespace utils
}  // namespace panel

#endif  // SRC_UTILS_LAYER_SHELL_HELPER_LAYER_SHELL_HELPER_H_
