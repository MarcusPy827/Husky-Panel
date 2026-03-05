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

#include <QPainter>
#include <QPainterPath>

#include "src/utils/misc/misc.h"

namespace panel {
namespace utils {

QPixmap Misc::ScalePixmap(const QPixmap &src, int w, int h) {
  if (src.isNull()) {
    return QPixmap();
  }
  return src.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QPixmap Misc::CropCirclePixmap(const QPixmap &src, int w, int h) {
  int size;
  if (w < 0 || h < 0) {
    size = qMin(src.width(), src.height());
  } else {
    size = qMin(w, h);
  }

  QPixmap target(size, size);
  target.fill(Qt::transparent);

  QPainter painter(&target);
  painter.setRenderHint(QPainter::Antialiasing);

  QPainterPath path;
  path.addEllipse(0, 0, size, size);
  painter.setClipPath(path);

  painter.drawPixmap(0, 0, src);
  return target;
}

}  // namespace utils
}  // namespace panel
