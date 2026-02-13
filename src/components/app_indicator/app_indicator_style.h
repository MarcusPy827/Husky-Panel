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

#ifndef SRC_COMPONENTS_APP_INDICATOR_APP_INDICATOR_STYLE_H_
#define SRC_COMPONENTS_APP_INDICATOR_APP_INDICATOR_STYLE_H_

#include <memory>

#include <QStringLiteral>
#include <QString>
#include <QMap>
#include <QList>

#include "src/utils/strings/strings.h"
#include "src/utils/color_palette_wrapper/color_palette_wrapper.h"
#include "src/utils/colors/colors.h"
#include "lib/3rdparty/material-color-utilities/cpp/dynamiccolor/dynamic_scheme.h"

namespace panel {
namespace loader {

static QString GetAppIndicatorPackageNameStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb text_color = palette->GetOnSurface();
  QString text_hex = utils::ColorPaletteWrapper::Argb2Hex(text_color);
  QString text_dim_hex = utils::Colors::ApplyOpacityToHexColor(text_hex, 0.7);
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QLabel[class='app_indicator_package_name_desc'] {
        color: %t1%;
        background: transparent;
        font-size: 12px;
      })"""), QList<QString>{text_dim_hex});
}

static QString GetAppIndicatorAppNameStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb text_color = palette->GetPrimary();
  QString text_hex = utils::ColorPaletteWrapper::Argb2Hex(text_color);
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QLabel[class='app_indicator_app_name_desc'] {
        color: %t1%;
        background: transparent;
        font-size: 12px;
        font-weight: bold;
      })"""), QList<QString>{text_hex});
}

static QString GetAppIndicatorStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  return GetAppIndicatorPackageNameStyle(palette)
    + GetAppIndicatorAppNameStyle(palette);
}

}  // namespace loader
}  // namespace panel

#endif  // SRC_COMPONENTS_APP_INDICATOR_APP_INDICATOR_STYLE_H_
