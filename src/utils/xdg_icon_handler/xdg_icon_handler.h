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

#ifndef SRC_UTILS_XDG_ICON_HANDLER_XDG_ICON_HANDLER_H_
#define SRC_UTILS_XDG_ICON_HANDLER_XDG_ICON_HANDLER_H_

namespace panel {
namespace utils {

class XdgIconHandler : public QQuickImageProvider {
 public:
  explicit XdgIconHandler();
  QPixmap requestPixmap(const QString& id, QSize* size,
    const QSize& requested) override;
};

}  // namespace utils
}  // namespace panel

#endif  // SRC_UTILS_XDG_ICON_HANDLER_XDG_ICON_HANDLER_H_
