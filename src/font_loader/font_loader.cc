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

#include <QDebug>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/font_loader/font_loader.h"

namespace panel {
namespace loader {

QFont FontLoader::GetOutlinedMaterialSymbolFont() {
  if (!is_outlined_symbols_loaded_) {
    LOG(INFO) << absl::StrCat("Loading outlined material symbol font...");
    outlined_symbol_id_ = QFontDatabase::addApplicationFont(
      ":/m3qw/fonts/fonts/material-symbols/material_symbol_outlined.ttf");
    is_outlined_symbols_loaded_ = true;

    LOG(INFO) << absl::StrCat("Outlined material symbol font has been loaded.");
  }

  QString family_gen =
    QFontDatabase::applicationFontFamilies(outlined_symbol_id_).at(0);
  return QFont(family_gen);
}

QFont FontLoader::GetRoundedMaterialSymbolFont() {
  if (!is_rounded_symbols_loaded_) {
    LOG(INFO) << absl::StrCat("Loading rounded material symbol font...");
    rounded_symbol_id_ = QFontDatabase::addApplicationFont(
      ":/m3qw/fonts/fonts/material-symbols/material_symbol_rounded.ttf");
    is_rounded_symbols_loaded_ = true;
    LOG(INFO) << absl::StrCat("Rounded material symbol font has been loaded.");
  }

  QString family_gen =
    QFontDatabase::applicationFontFamilies(rounded_symbol_id_).at(0);
  return QFont(family_gen);
}

QFont FontLoader::GetSharpMaterialSymbolFont() {
  if (!is_sharp_symbols_loaded_) {
    LOG(INFO) << absl::StrCat("Loading sharp material symbol font...");
    sharp_symbol_id_ = QFontDatabase::addApplicationFont(
      ":/m3qw/fonts/fonts/material-symbols/material_symbol_sharp.ttf");
    is_sharp_symbols_loaded_ = true;
    LOG(INFO) << absl::StrCat("Sharp material symbol font has been loaded.");
  }

  QString family_gen =
    QFontDatabase::applicationFontFamilies(sharp_symbol_id_).at(0);
  return QFont(family_gen);
}

}  // namespace loader
}  // namespace panel
