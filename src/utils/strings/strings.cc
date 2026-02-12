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

#include "src/utils/strings/strings.h"

namespace flake {
namespace utils {

QString Strings::TemplateCat(QString original, QList<QString> args) {
  for (int i = 0; i < args.size(); i++) {
    const QString current_arg = args.at(i);
    const QString current_template_placeholder = "%t" + QString::number(i + 1)
      + "%";
    original.replace(current_template_placeholder, current_arg);
  }
  return original;
}

}  // namespace utils
}  // namespace flake
