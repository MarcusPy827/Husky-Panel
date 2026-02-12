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

#ifndef SRC_THEME_LOADER_THEME_CONF_H_
#define SRC_THEME_LOADER_THEME_CONF_H_

#include <memory>

#include <QStringLiteral>
#include <QString>
#include <QMap>
#include <QList>

#include "src/utils/strings/strings.h"
#include "src/utils/color_palette_wrapper/color_palette_wrapper.h"
#include "src/utils/colors/colors.h"
#include "src/components/app_indicator/app_indicator_style.h"
#include "lib/3rdparty/material-color-utilities/cpp/dynamiccolor/dynamic_scheme.h"

namespace panel {
namespace loader {

struct StyleTuple {
  QString light_style;
  QString dark_style;
};

struct SchemeTuple {
  std::unique_ptr<material_color_utilities::DynamicScheme> light;
  std::unique_ptr<material_color_utilities::DynamicScheme> dark;
};

static QString GetAppBackground(
  const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb bg_color = palette->GetSurfaceContainer();
  QString bg_hex = utils::ColorPaletteWrapper::Argb2Hex(bg_color);
  material_color_utilities::Argb border_color = palette->GetPrimary();
  QString border_hex = utils::ColorPaletteWrapper::Argb2Hex(border_color);

  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      *[class='base_layer'] {
        background: %t1%;
        border: 0px solid %t2%;
        border-top-left-radius: 8px;
        border-top-right-radius: 8px;
        border-bottom-left-radius: 0px;
        border-bottom-right-radius: 0px;
      })"""), QList<QString>{bg_hex, border_hex});
}

static QString GetCommonBtnStyle(
  const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb text_color = palette->GetOnSurface();
  QString text_hex = utils::ColorPaletteWrapper::Argb2Hex(text_color);
  material_color_utilities::Argb state_layer_color = palette
    ->GetOnSurfaceVariant();
  QString state_layer_hex_raw = utils::ColorPaletteWrapper::Argb2Hex(
    state_layer_color, true);
  QString state_layer_hex_hover = utils::Colors::ApplyOpacityToHexColor(
    state_layer_hex_raw, 0.08);
  QString state_layer_hex_pressed = utils::Colors::ApplyOpacityToHexColor(
    state_layer_hex_raw, 0.1);

  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QPushButton[class='common_bar_btn'] {
        color: %t1%;
        background: transparent;
        border: 0px solid %t1%;
        border-radius: 6px;
        font-size: 16px;
        height: 32px;
        padding-left: 8px;
        padding-right: 8px;
      }
        
      QPushButton[class='common_bar_btn']:hover {
        color: %t1%;
        background: %t2%;
        border: 0px solid %t2%;
        border-radius: 6px;
        font-size: 16px;
        height: 32px;
        padding-left: 8px;
        padding-right: 8px;
      }
        
      QPushButton[class='common_bar_btn']:pressed {
        color: %t1%;
        background: %t3%;
        border: 0px solid %t3%;
        border-radius: 6px;
        font-size: 16px;
        height: 32px;
        padding-left: 8px;
        padding-right: 8px;
      })"""), QList<QString>{text_hex, state_layer_hex_hover,
                             state_layer_hex_pressed});
}

static StyleTuple GetThemeMap(const SchemeTuple& conf) {
  return {
    GetAppBackground(conf.dark)
      + GetAppIndicatorStyle(conf.dark)
      + GetCommonBtnStyle(conf.dark),
    GetAppBackground(conf.dark)
      + GetAppIndicatorStyle(conf.dark)
      + GetCommonBtnStyle(conf.dark)
  };
}

}  // namespace loader
}  // namespace panel

#endif  // SRC_THEME_LOADER_THEME_CONF_H_
