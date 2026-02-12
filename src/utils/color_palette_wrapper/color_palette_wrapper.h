/* Copyright (C) 2026 MarcusPy827
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the Free 
 * Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SRC_UTILS_COLOR_PALETTE_WRAPPER_COLOR_PALETTE_WRAPPER_H_
#define SRC_UTILS_COLOR_PALETTE_WRAPPER_COLOR_PALETTE_WRAPPER_H_

#include <memory>

#include <QColor>
#include <QMap>
#include <QString>

#include "lib/3rdparty/material-color-utilities/cpp/palettes/core.h"
#include "lib/3rdparty/material-color-utilities/cpp/palettes/tones.h"
#include "lib/3rdparty/material-color-utilities/cpp/dynamiccolor/dynamic_scheme.h"

namespace flake {
namespace utils {

enum class ColorSchemeType {
  kExpressive,
  kFidelity,
  kFruitSalad,
  kMonochrome,
  kNeutral,
  kVibrant,
  kRainbow,
  kTonalSpot
};

class ColorPaletteWrapper {
 public:
  static QColor GetSystemHighlightColor();
  static material_color_utilities::Argb Hex2Rgb(const QString& hex);
  static QString Argb2Hex(material_color_utilities::Argb argb,
    bool read_alpha = true);
  static material_color_utilities::CorePalettes GenPalette(const QString& hex);
  static std::unique_ptr<material_color_utilities::DynamicScheme> GenScheme(
    const QString& primary_hex, ColorSchemeType type, bool is_dark = false,
    double contrast_level = 0.0);
  static QString GetTypeName(ColorSchemeType type);
  static bool IsDark();
};

}  // namespace utils
}  // namespace flake

#endif  // SRC_UTILS_COLOR_PALETTE_WRAPPER_COLOR_PALETTE_WRAPPER_H_
