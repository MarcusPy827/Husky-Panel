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

#include <QColor>
#include <QDebug>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/utils/colors/colors.h"
#include "src/utils/strings/strings.h"

namespace panel {
namespace utils {

QString Colors::ApplyOpacityToHexColor(const QString& hex,
    const double & opacity) {
  QColor color(hex);
  if (color.isValid()) {
    int r = color.red();
    int g = color.green();
    int b = color.blue();

    return Strings::TemplateCat(QStringLiteral(
      R"""(rgba(%t1%, %t2%, %t3%, %t4%))"""),
      QList<QString>{
        QString::number(r), QString::number(g), QString::number(b),
        QString::number(opacity)});
  } else {
    LOG(ERROR) << absl::StrCat(
      "Hex color util: Cannot apply opacity to hex color", hex.toStdString(),
      "as it is invalid, no action taken.");
    return hex;
  }
}

}  // namespace utils
}  // namespace panel
