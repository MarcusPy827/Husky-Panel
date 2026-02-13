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
#include <QApplication>
#include <QStyleHints>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/theme_loader/theme_loader.h"
#include "src/utils/color_palette_wrapper/color_palette_wrapper.h"

namespace panel {
namespace loader {

ThemeLoader::ThemeLoader(QWidget * base_layer, QString base_color,
    QWidget * parent) : QObject(parent) {
  if (base_layer == nullptr) {
    LOG(ERROR) << absl::StrCat(
      "No valid base layer, skipping loading styles...");
    return;
  }

  install_target_ = base_layer;
  palette_conf_.dark = utils::ColorPaletteWrapper::GenScheme(base_color,
    utils::ColorSchemeType::kTonalSpot, true);
  palette_conf_.light = utils::ColorPaletteWrapper::GenScheme(base_color,
    utils::ColorSchemeType::kTonalSpot, false);
  theme_map_ = GetThemeMap(palette_conf_);
  LoadThemeFromMap(IsDarkMode());

  connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this,
      [this](Qt::ColorScheme scheme){
    LOG(INFO) << absl::StrCat("Detected color scheme has been changed,",
      " now reloading theme...");
    LoadThemeFromMap(IsDarkMode());
  });
}

void ThemeLoader::LoadThemeFromMap(bool use_dark_mode) {
  if (install_target_ != nullptr) {
    LOG(INFO) << absl::StrCat("Loading sector styles...");
    install_target_->setStyleSheet(use_dark_mode ? theme_map_.dark_style :
      theme_map_.light_style);
  } else {
    LOG(ERROR) << absl::StrCat("No parent widget was specified during ",
      "initialization. Selector styles will NOT be loaded.");
  }
}

bool ThemeLoader::IsDarkMode() {
  return (qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark);
}

}  // namespace loader
}  // namespace panel
