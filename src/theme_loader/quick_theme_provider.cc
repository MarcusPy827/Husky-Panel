/*
 * Copyright (C) 2026 MarcusPy827
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

#include <memory>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/theme_loader/quick_theme_provider.h"
#include "src/utils/color_palette_wrapper/color_palette_wrapper.h"
#include "src/utils/colors/colors.h"

namespace panel {
namespace loader {

QuickThemeProvider::QuickThemeProvider(QObject* parent) : QObject(parent) {
  LOG(INFO) << absl::StrCat("QuickThemeProvider constructer invoked!!");
  RefreshTheme();
}

QuickThemeProvider::~QuickThemeProvider() {
  LOG(INFO) << absl::StrCat("QuickThemeProvider destructor invoked!!");
}

void QuickThemeProvider::RefreshTheme() {
  LOG(INFO) << absl::StrCat("Now refreshing palette for QML...");
  QColor highlight_color_raw =
    utils::ColorPaletteWrapper::GetSystemHighlightColor();
  QString highlight_color = highlight_color_raw.name();
  bool is_dark = utils::ColorPaletteWrapper::IsDark();

  std::unique_ptr<material_color_utilities::DynamicScheme> scheme_gen =
    utils::ColorPaletteWrapper::GenScheme(
      highlight_color, utils::ColorSchemeType::kTonalSpot, is_dark);

  if (!is_dark) {
    std::unique_ptr<material_color_utilities::DynamicScheme> temp_dark_scheme =
      utils::ColorPaletteWrapper::GenScheme(
        highlight_color, utils::ColorSchemeType::kTonalSpot, true);
    if (WriteSchemeForStatusBar(*temp_dark_scheme) == 0) {
      WriteScheme(*scheme_gen);
    }
  } else {
    if (WriteSchemeForStatusBar(*scheme_gen) == 0) {
      WriteScheme(*scheme_gen);
    }
  }
}

void QuickThemeProvider::WriteScheme(const
    material_color_utilities::DynamicScheme & scheme) {
  primary_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetPrimary()));
  surface_container_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetSurfaceContainer()));
  surface_fg_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetOnSurface()));

  QColor state_layer_base = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetOnSurfaceVariant()));
  state_layer_hover_ = state_layer_base;
  state_layer_hover_.setAlphaF(0.08f);
  state_layer_pressed_ = state_layer_base;
  state_layer_pressed_.setAlphaF(0.10f);

  surface_bg_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetSurface()));
  surface_container_low_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetSurfaceContainerLow()));
  surface_container_lowest_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetSurfaceContainerLowest()));
  surface_container_high_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetSurfaceContainerHigh()));
  surface_variant_fg_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetOnSurfaceVariant()));
  primary_container_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetPrimaryContainer()));
  secondary_container_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetSecondaryContainer()));
  secondary_container_fg_ = QColor(
    utils::ColorPaletteWrapper::Argb2Hex(
      scheme.GetOnSecondaryContainer()));

  emit SchemeUpdated();
}

int QuickThemeProvider::WriteSchemeForStatusBar(const
    material_color_utilities::DynamicScheme & scheme) {
  status_bar_bg_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetSurfaceContainer()));
  status_bar_primary_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetPrimary()));
  status_bar_surface_fg_ = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetOnSurface()));
  QColor state_layer_base = QColor(utils::ColorPaletteWrapper::Argb2Hex(
    scheme.GetOnSurfaceVariant()));
  status_bar_state_layer_hover_ = state_layer_base;
  status_bar_state_layer_hover_.setAlphaF(0.08f);
  status_bar_state_layer_pressed_ = state_layer_base;
  status_bar_state_layer_pressed_.setAlphaF(0.10f);
  return 0;
}

const QColor QuickThemeProvider::GetStatusBarBg() {
  return status_bar_bg_;
}

const QColor QuickThemeProvider::GetStatusBarPrimary() {
  return status_bar_primary_;
}

const QColor QuickThemeProvider::GetStatusBarSurfaceFg() {
  return status_bar_surface_fg_;
}

const QColor QuickThemeProvider::GetStatusBarStateLayerHover() {
  return status_bar_state_layer_hover_;
}

const QColor QuickThemeProvider::GetStatusBarStateLayerPressed() {
  return status_bar_state_layer_pressed_;
}

const QColor QuickThemeProvider::GetPrimary() {
  return primary_;
}

const QColor QuickThemeProvider::GetSurfaceContainer() {
  return surface_container_;
}

const QColor QuickThemeProvider::GetSurfaceFg() {
  return surface_fg_;
}

const QColor QuickThemeProvider::GetStateLayerHover() {
  return state_layer_hover_;
}

const QColor QuickThemeProvider::GetStateLayerPressed() {
  return state_layer_pressed_;
}

const QColor QuickThemeProvider::GetSurfaceBg() {
  return surface_bg_;
}

const QColor QuickThemeProvider::GetSurfaceContainerLow() {
  return surface_container_low_;
}

const QColor QuickThemeProvider::GetSurfaceContainerLowest() {
  return surface_container_lowest_;
}

const QColor QuickThemeProvider::GetSurfaceContainerHigh() {
  return surface_container_high_;
}

const QColor QuickThemeProvider::GetSurfaceVariantFg() {
  return surface_variant_fg_;
}

const QColor QuickThemeProvider::GetPrimaryContainer() {
  return primary_container_;
}

const QColor QuickThemeProvider::GetSecondaryContainer() {
  return secondary_container_;
}

const QColor QuickThemeProvider::GetSecondaryContainerFg() {
  return secondary_container_fg_;
}

}  // namespace loader
}  // namespace panel
