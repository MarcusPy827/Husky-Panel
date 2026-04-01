/*
 * tray_icon_image_handler.cc
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

#include "src/components/system_tray/tray_icon_image_handler.h"

namespace panel {
namespace frontend {

TrayIconImageHandler::TrayIconImageHandler(SystemTrayHandler* handler)
    : QQuickImageProvider(QQuickImageProvider::Image),
      handler_(handler) {}

QImage TrayIconImageHandler::requestImage(const QString& id, QSize* size,
    const QSize& requested_size) {
  // id format: "<key>/<revision>"  — split on first '/'.
  int sep = id.indexOf('/');
  bool ok = false;
  int key = (sep > 0 ? id.left(sep) : id).toInt(&ok);
  if (!ok || !handler_) {
    if (size) *size = QSize(1, 1);
    return QImage(1, 1, QImage::Format_ARGB32);
  }

  QImage img = handler_->GetIconImage(key);
  if (img.isNull()) {
    if (size) *size = QSize(1, 1);
    return QImage(1, 1, QImage::Format_ARGB32);
  }

  if (requested_size.isValid() && requested_size != img.size())
    img = img.scaled(requested_size, Qt::KeepAspectRatio,
      Qt::SmoothTransformation);

  if (size) *size = img.size();
  return img;
}

}  // namespace frontend
}  // namespace panel
