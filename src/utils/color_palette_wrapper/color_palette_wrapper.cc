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

#include <vector>
#include <algorithm>
#include <memory>

#include <QApplication>
#include <QPalette>
#include <QStyleHints>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/utils/color_palette_wrapper/color_palette_wrapper.h"

#include "lib/3rdparty/material-color-utilities/cpp/dynamiccolor/dynamic_scheme.h"
#include "lib/3rdparty/material-color-utilities/cpp/scheme/scheme_expressive.h"
#include "lib/3rdparty/material-color-utilities/cpp/scheme/scheme_fidelity.h"
#include "lib/3rdparty/material-color-utilities/cpp/scheme/scheme_fruit_salad.h"
#include "lib/3rdparty/material-color-utilities/cpp/scheme/scheme_monochrome.h"
#include "lib/3rdparty/material-color-utilities/cpp/scheme/scheme_neutral.h"
#include "lib/3rdparty/material-color-utilities/cpp/scheme/scheme_vibrant.h"
#include "lib/3rdparty/material-color-utilities/cpp/scheme/scheme_rainbow.h"
#include "lib/3rdparty/material-color-utilities/cpp/scheme/scheme_tonal_spot.h"

namespace flake {
namespace utils {

QColor ColorPaletteWrapper::GetSystemHighlightColor() {
  LOG(INFO) << absl::StrCat("Now fetching system highlight color...");
  const QPalette& palette = QApplication::palette();
  const QColor& result = palette.color(QPalette::Active, QPalette::Highlight);
  LOG(INFO) << absl::StrCat("Successfully read system highlight color: ",
    result.name().toStdString(), ".");
  return result;
}

material_color_utilities::Argb ColorPaletteWrapper::Hex2Rgb(
    const QString& hex) {
  QString raw = hex.trimmed();
  if (raw.startsWith("#")) {
    raw = raw.mid(1);
  }

  if (raw.length() == 6) {
    raw = "FF" + raw;
  } else if (raw.length() == 8) {
    // RRGGBBAA → AARRGGBB
    raw = raw.mid(6) + raw.mid(0, 6);
  }

  bool ok = false;
  uint32_t result = raw.toUInt(&ok, 16);

  if (!ok) {
    LOG(ERROR) << absl::StrCat("Failed to parse hex string \"",
      hex.toStdString(), "\". Now defaulting to 0xFF000000...");
    return static_cast<material_color_utilities::Argb>(0xFF000000);
  }
  return static_cast<material_color_utilities::Argb>(result);
}

QString ColorPaletteWrapper::Argb2Hex(material_color_utilities::Argb argb,
    bool read_alpha) {
  uint32_t raw = static_cast<uint32_t>(argb);
  QString result;

  if (read_alpha) {
    // AARRGGBB → #RRGGBBAA
    uint8_t alpha = (raw >> 24) & 0xFF;
    uint8_t red = (raw >> 16) & 0xFF;
    uint8_t green = (raw >> 8) & 0xFF;
    uint8_t blue = raw & 0xFF;

    result = QString("#%1%2%3%4")
      .arg(alpha, 2, 16, QChar('0'))
      .arg(red, 2, 16, QChar('0'))
      .arg(green, 2, 16, QChar('0'))
      .arg(blue, 2, 16, QChar('0'));
  } else {
    // AARRGGBB → #RRGGBB
    uint8_t red = (raw >> 16) & 0xFF;
    uint8_t green = (raw >> 8) & 0xFF;
    uint8_t blue = raw & 0xFF;

    result = QString("#%1%2%3")
      .arg(red, 2, 16, QChar('0'))
      .arg(green, 2, 16, QChar('0'))
      .arg(blue, 2, 16, QChar('0'));
  }

  return result.toUpper();
}

material_color_utilities::CorePalettes ColorPaletteWrapper::GenPalette(
    const QString& hex) {
  LOG(INFO) << absl::StrCat("Generating core palettes from hex \"",
    hex.toStdString(), "\".");

  material_color_utilities::Argb argb = Hex2Rgb(hex);
  material_color_utilities::Hct hct(argb);
  double hue = hct.get_hue();
  double chroma = hct.get_chroma();

  LOG(INFO) << absl::StrCat("Generated palettes successfully.");

  return {
    material_color_utilities::TonalPalette(hue, 36.0),
    material_color_utilities::TonalPalette(hue, 16.0),
    material_color_utilities::TonalPalette(hue + 60.0, 24.0),
    material_color_utilities::TonalPalette(hue, 6.0),
    material_color_utilities::TonalPalette(hue, 8.0)
  };
}

std::unique_ptr<material_color_utilities::DynamicScheme>
    ColorPaletteWrapper::GenScheme(const QString& primary_hex,
    ColorSchemeType type, bool is_dark, double contrast_level) {
  LOG(INFO) << absl::StrCat("Generating dynamic scheme from hex \"",
    primary_hex.toStdString(), "\" with type\"",
    GetTypeName(type).toStdString(), "\".");

  material_color_utilities::Argb argb = Hex2Rgb(primary_hex);
  material_color_utilities::Hct hct(argb);

  switch (type) {
    case ColorSchemeType::kExpressive: {
      return std::make_unique<material_color_utilities::SchemeExpressive>(
        hct, is_dark, contrast_level);
    }
    case ColorSchemeType::kFidelity: {
      return std::make_unique<material_color_utilities::SchemeFidelity>(
        hct, is_dark, contrast_level);
      }
    case ColorSchemeType::kFruitSalad: {
      return std::make_unique<material_color_utilities::SchemeFruitSalad>(
        hct, is_dark, contrast_level);
      }
    case ColorSchemeType::kMonochrome: {
      return std::make_unique<material_color_utilities::SchemeMonochrome>(
        hct, is_dark, contrast_level);
      }
    case ColorSchemeType::kNeutral: {
      return std::make_unique<material_color_utilities::SchemeNeutral>(
        hct, is_dark, contrast_level);
    }
    case ColorSchemeType::kVibrant: {
      return std::make_unique<material_color_utilities::SchemeVibrant>(
        hct, is_dark, contrast_level);
    }
    case ColorSchemeType::kRainbow: {
      return std::make_unique<material_color_utilities::SchemeRainbow>(
        hct, is_dark, contrast_level);
    }
    case ColorSchemeType::kTonalSpot: {
      return std::make_unique<material_color_utilities::SchemeTonalSpot>(
        hct, is_dark, contrast_level);
    }
    default: {
      LOG(ERROR) << absl::StrCat("Unknown ColorSchemeType \"",
        GetTypeName(type).toStdString(), "\", defaulting to \"",
        GetTypeName(ColorSchemeType::kTonalSpot).toStdString(), "\"...");
      return std::make_unique<material_color_utilities::SchemeTonalSpot>(
          hct, is_dark, contrast_level);
    }
  }
}

QString ColorPaletteWrapper::GetTypeName(ColorSchemeType type) {
  switch (type) {
    case ColorSchemeType::kExpressive: {
      return "ColorSchemeType::kExpressive";
    }
    case ColorSchemeType::kFidelity: {
      return "ColorSchemeType::kFidelity";
    }
    case ColorSchemeType::kFruitSalad: {
      return "ColorSchemeType::kFruitSalad";
    }
    case ColorSchemeType::kMonochrome: {
      return "ColorSchemeType::kMonochrome";
    }
    case ColorSchemeType::kNeutral: {
      return "ColorSchemeType::kNeutral";
    }
    case ColorSchemeType::kVibrant: {
      return "ColorSchemeType::kVibrant";
    }
    case ColorSchemeType::kRainbow: {
      return "ColorSchemeType::kRainbow";
    }
    case ColorSchemeType::kTonalSpot: {
      return "ColorSchemeType::kTonalSpot";
    }
  }

  LOG(ERROR) << "Unknown ColorSchemeType: " << static_cast<int>(type)
    << ", you may want to check your code.";
  return "Unknown";
}

bool ColorPaletteWrapper::IsDark() {
  return (qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark);
}

}  // namespace utils
}  // namespace flake
