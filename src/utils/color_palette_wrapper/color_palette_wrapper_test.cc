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

#include <QString>

#include "gtest/gtest.h"
#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include <cmath>

#include "src/utils/color_palette_wrapper/color_palette_wrapper.h"

#include "lib/3rdparty/material-color-utilities/cpp/scheme/scheme_expressive.h"
#include "lib/3rdparty/material-color-utilities/cpp/scheme/scheme_tonal_spot.h"

namespace panel {
namespace eval {
namespace {

double epsilon = 1e-6;

bool VerifyValidHex(const QString& hex) {
  LOG(INFO) << absl::StrCat("Verifying hex string: ", hex.toStdString(), ".");
  QString sanitized;
  if (hex.startsWith("#")) {
    if (hex.length() != 7 && hex.length() != 9) {
      LOG(ERROR) << absl::StrCat("Invalid hex string length (with #): ",
        hex.length(), ".");
      return false;
    }
    sanitized = hex.mid(1);
  } else {
    if (hex.length() != 6 && hex.length() != 8) {
      LOG(ERROR) << absl::StrCat("Invalid hex string length (without #): ",
        hex.length(), ".");
      return false;
    }
    sanitized = hex;
  }

  for (const QChar& current_char : sanitized) {
    if (current_char >= '0' && current_char <= '9') {
      LOG(ERROR) << "Invalid character in hex string: "
        << current_char.toLatin1() << ".";
      continue;
    }
    if (current_char >= 'A' && current_char <= 'F') {
      LOG(ERROR) << "Invalid character in hex string: "
        << current_char.toLatin1() << ".";
      continue;
    }
    if (current_char >= 'a' && current_char <= 'f') {
      LOG(ERROR) << "Invalid character in hex string: "
        << current_char.toLatin1() << ".";
      continue;
    }
    return false;
  }

  LOG(INFO) << absl::StrCat("Hex string ", hex.toStdString(),
    " is valid.");
  return true;
}
}  // namespace

TEST(SystemAccentColorTest, TestHexValid) {
  QString result_gen = utils::ColorPaletteWrapper::GetSystemHighlightColor()
    .name();
  EXPECT_TRUE(VerifyValidHex(result_gen));
}

TEST(Hex2ARgbFormatTest, TestCastValid) {
  auto hex_2_rgb_gen_1 = utils::ColorPaletteWrapper::Hex2Rgb("#009688");
  auto hex_2_rgb_ground_truth_1 = static_cast<material_color_utilities::Argb>(
    0xFF009688u);
  auto hex_2_rgb_gen_2 = utils::ColorPaletteWrapper::Hex2Rgb("#2196F3");
  auto hex_2_rgb_ground_truth_2 = static_cast<material_color_utilities::Argb>(
    0xFF2196F3u);
  auto hex_2_rgb_gen_3 = utils::ColorPaletteWrapper::Hex2Rgb("#00968880");
  auto hex_2_rgb_ground_truth_3 = static_cast<material_color_utilities::Argb>(
    0x80009688u);
  auto hex_2_rgb_gen_4 = utils::ColorPaletteWrapper::Hex2Rgb("#2196F380");
  auto hex_2_rgb_ground_truth_4 = static_cast<material_color_utilities::Argb>(
    0x802196F3u);
  auto hex_2_rgb_gen_5 = utils::ColorPaletteWrapper::Hex2Rgb("#2196f3");
  auto hex_2_rgb_ground_truth_5 = static_cast<material_color_utilities::Argb>(
    0xFF2196F3u);

  EXPECT_EQ(hex_2_rgb_gen_1, hex_2_rgb_ground_truth_1);
  EXPECT_EQ(hex_2_rgb_gen_2, hex_2_rgb_ground_truth_2);
  EXPECT_EQ(hex_2_rgb_gen_3, hex_2_rgb_ground_truth_3);
  EXPECT_EQ(hex_2_rgb_gen_4, hex_2_rgb_ground_truth_4);
  EXPECT_EQ(hex_2_rgb_gen_5, hex_2_rgb_ground_truth_5);
}

TEST(ColorPaletteWrapperTest, InvalidHexInput) {
  auto invalid_hex_gen = utils::ColorPaletteWrapper::Hex2Rgb("NotHex");
  auto invalid_hex_ground_truth = static_cast<material_color_utilities::Argb>(
    0xFF000000u);
  EXPECT_EQ(invalid_hex_gen, invalid_hex_ground_truth);
}

TEST(Argb2HexFormatTest, TestHexValid) {
  auto argb_2_hex_raw_1 = static_cast<material_color_utilities::Argb>(
    0xFF009688u);
  auto argb_2_hex_gen_1 = utils::ColorPaletteWrapper::Argb2Hex(
    argb_2_hex_raw_1, true);

  auto argb_2_hex_raw_2 = static_cast<material_color_utilities::Argb>(
    0xFF2196F3u);
  auto argb_2_hex_gen_2 = utils::ColorPaletteWrapper::Argb2Hex(
    argb_2_hex_raw_2, true);

  EXPECT_EQ(argb_2_hex_gen_1, "#FF009688");
  EXPECT_EQ(argb_2_hex_gen_2, "#FF2196F3");
}

TEST(ColorPaletteWrapperTest, GenPaletteTest) {
  const QString hex = "#2196F3";
  auto palettes = utils::ColorPaletteWrapper::GenPalette(hex);
  auto hct = ::material_color_utilities::Hct(
    utils::ColorPaletteWrapper::Hex2Rgb(hex));

  EXPECT_NEAR(palettes.primary.get_hue(), hct.get_hue(), epsilon);
  EXPECT_NEAR(palettes.primary.get_chroma(), 36.0, epsilon);
  EXPECT_NEAR(palettes.secondary.get_chroma(), 16.0, epsilon);
  EXPECT_NEAR(std::fmod(palettes.tertiary.get_hue(), 360.0),
    std::fmod(hct.get_hue() + 60.0, 360.0), epsilon);
  EXPECT_NEAR(palettes.neutral.get_chroma(), 6.0, epsilon);
  EXPECT_NEAR(palettes.neutral_variant.get_chroma(), 8.0, epsilon);
}

TEST(ColorPaletteWrapperTest, SchemeTypeDetectTest) {
  auto scheme_type_expressive_gen = utils::ColorPaletteWrapper::GetTypeName(
    utils::ColorSchemeType::kExpressive);
  auto scheme_type_expressive_ground_truth = QString(
    "ColorSchemeType::kExpressive");
  auto scheme_type_fidelity_gen = utils::ColorPaletteWrapper::GetTypeName(
    utils::ColorSchemeType::kFidelity);
  auto scheme_type_fidelity_ground_truth = QString(
    "ColorSchemeType::kFidelity");
  auto scheme_type_fruit_salad_gen = utils::ColorPaletteWrapper::GetTypeName(
    utils::ColorSchemeType::kFruitSalad);
  auto scheme_type_fruit_salad_ground_truth = QString(
    "ColorSchemeType::kFruitSalad");
  auto scheme_type_monochrome_gen = utils::ColorPaletteWrapper::GetTypeName(
    utils::ColorSchemeType::kMonochrome);
  auto scheme_type_monochrome_ground_truth = QString(
    "ColorSchemeType::kMonochrome");
  auto scheme_type_neutral_gen = utils::ColorPaletteWrapper::GetTypeName(
    utils::ColorSchemeType::kNeutral);
  auto scheme_type_neutral_ground_truth = QString(
    "ColorSchemeType::kNeutral");
  auto scheme_type_vibrant_gen = utils::ColorPaletteWrapper::GetTypeName(
    utils::ColorSchemeType::kVibrant);
  auto scheme_type_vibrant_ground_truth = QString(
    "ColorSchemeType::kVibrant");
  auto scheme_type_rainbow_gen = utils::ColorPaletteWrapper::GetTypeName(
    utils::ColorSchemeType::kRainbow);
  auto scheme_type_rainbow_ground_truth = QString(
    "ColorSchemeType::kRainbow");
  auto scheme_type_tonal_spot_gen = utils::ColorPaletteWrapper::GetTypeName(
    utils::ColorSchemeType::kTonalSpot);
  auto scheme_type_tonal_spot_ground_truth = QString(
    "ColorSchemeType::kTonalSpot");

  EXPECT_EQ(scheme_type_expressive_gen, scheme_type_expressive_ground_truth);
  EXPECT_EQ(scheme_type_fidelity_gen, scheme_type_fidelity_ground_truth);
  EXPECT_EQ(scheme_type_fruit_salad_gen, scheme_type_fruit_salad_ground_truth);
  EXPECT_EQ(scheme_type_monochrome_gen, scheme_type_monochrome_ground_truth);
  EXPECT_EQ(scheme_type_neutral_gen, scheme_type_neutral_ground_truth);
  EXPECT_EQ(scheme_type_vibrant_gen, scheme_type_vibrant_ground_truth);
  EXPECT_EQ(scheme_type_rainbow_gen, scheme_type_rainbow_ground_truth);
  EXPECT_EQ(scheme_type_tonal_spot_gen, scheme_type_tonal_spot_ground_truth);
}

}  // namespace eval
}  // namespace panel
