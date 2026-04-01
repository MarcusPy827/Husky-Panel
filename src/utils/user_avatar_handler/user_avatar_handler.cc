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

#include "src/utils/user_avatar_handler/user_avatar_handler.h"
#include "src/utils/misc/misc.h"
#include "src/info_server/user_info/user_info.h"

namespace panel {
namespace utils {

UserAvatarHandler::UserAvatarHandler() : QQuickImageProvider(
    QQuickImageProvider::Pixmap) {
  LOG(INFO) << absl::StrCat("UserAvatarHandler constructer invoked!!");
}

QPixmap UserAvatarHandler::requestPixmap(const QString& id, QSize* size,
    const QSize& requested) {
  int size_read = 48;
  if (requested.isValid() && requested.width() > 0) {
    size_read = requested.width();
  }

  if (size != nullptr) {
    *size = QSize(size_read, size_read);
  }

  QString path = backend::UserInfo::GetUserAvatarPath();
  if (path.isEmpty()) {
    QPixmap empty(size_read, size_read);
    empty.fill(Qt::transparent);
    return empty;
  }

  QPixmap src = panel::utils::Misc::ScalePixmap(QPixmap(path), size_read,
    size_read);
  return panel::utils::Misc::CropCirclePixmap(src, size_read, size_read);
}

}  // namespace utils
}  // namespace panel
