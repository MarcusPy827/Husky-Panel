/*
 * tray_icon_image_handler.h
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

#ifndef SRC_COMPONENTS_SYSTEM_TRAY_TRAY_ICON_IMAGE_HANDLER_H_
#define SRC_COMPONENTS_SYSTEM_TRAY_TRAY_ICON_IMAGE_HANDLER_H_

#include <QQuickImageProvider>

#include "src/components/system_tray/system_tray_handler.h"

namespace panel {
namespace frontend {

// QQuickImageProvider that serves tray icon images under the "trayicon" id.
// URL format: image://trayicon/<key>/<revision>
// <key>      — stable integer assigned per service in SystemTrayHandler.
// <revision> — increments each time the icon changes, busting the QML cache.
// requestImage() may be called from the render thread; SystemTrayHandler
// protects its image map with a QReadWriteLock.
class TrayIconImageHandler : public QQuickImageProvider {
 public:
  explicit TrayIconImageHandler(SystemTrayHandler* handler);

  /* ---------- For backend usage ---------- */
  QImage requestImage(const QString& id, QSize* size,
    const QSize& requested_size) override;

 private:
  SystemTrayHandler* handler_;
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_SYSTEM_TRAY_TRAY_ICON_IMAGE_HANDLER_H_
