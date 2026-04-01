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

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/utils/xdg_icon_handler/xdg_icon_handler.h"

namespace panel {
namespace utils {

XdgIconHandler::XdgIconHandler() : QQuickImageProvider(
    QQuickImageProvider::Pixmap) {
  LOG(INFO) << absl::StrCat("XdgIconHandler constructer invoked!!");
}

QPixmap XdgIconHandler::requestPixmap(const QString& id, QSize* size,
    const QSize& requested) {
  QIcon icon = QIcon::fromTheme(id);
  int size_read = 48;
  if (requested.isValid() && requested.width() > 0) {
    size_read = requested.width();
  }

  if (size != nullptr) {
    *size = QSize(size_read, size_read);
  }

  return icon.pixmap(size_read, size_read);
}

}  // namespace utils
}  // namespace panel
