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

#ifndef SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_STYLE_H_
#define SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_STYLE_H_

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

static QString GetAppDrawerBackground(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb bg_color = palette->GetSurface();
  QString bg_hex = utils::ColorPaletteWrapper::Argb2Hex(bg_color);
  material_color_utilities::Argb border_color = palette->GetPrimary();
  QString border_hex = utils::ColorPaletteWrapper::Argb2Hex(border_color);
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QWidget[class='calendar_frame'] {
        background: %t1%;
        border: 2px solid %t2%;
        outline: 2px solid %t2%;
        border-radius: 12px;
        padding: 10px;
      })"""), QList<QString>{bg_hex, border_hex});
}

static QString GetAppDarwerAppBarBaseStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb bg_color = palette->GetSurfaceContainer();
  QString bg_hex = utils::ColorPaletteWrapper::Argb2Hex(bg_color);
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QWidget[class='app_bar_container_app_drawer'] {
        min-height: 64px;
        max-height: 64px;
        background: %t1%;
        border: 0px solid %t1%;
        border-top-left-radius: 12px;
        border-top-right-radius: 12px;
        border-bottom-left-radius: 0px;
        border-bottom-right-radius: 0px;
        margin: 0px;
        padding: 0px;
      })"""), QList<QString>{bg_hex});
}

static QString GetAppDarwerToolBarStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb bg_color = palette->GetSurfaceContainer();
  QString bg_hex = utils::ColorPaletteWrapper::Argb2Hex(bg_color);
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QWidget[class='tool_bar_app_drawer'] {
        min-height: 64px;
        max-height: 64px;
        background: %t1%;
        border: 0px solid %t1%;
        border-top-left-radius: 0px;
        border-top-right-radius: 0px;
        border-bottom-left-radius: 12px;
        border-bottom-right-radius: 12px;
        margin: 0px;
        padding: 0px;
      })"""), QList<QString>{bg_hex});
}

static QString GetAppDarwerSidePaneItemStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb bg_color = palette->GetSecondaryContainer();
  QString bg_hex = utils::ColorPaletteWrapper::Argb2Hex(bg_color);
  material_color_utilities::Argb icon_color = palette
    ->GetOnSurfaceVariant();
  QString icon_hex = utils::ColorPaletteWrapper::Argb2Hex(icon_color);
  material_color_utilities::Argb active_icon_color = palette
    ->GetOnSecondaryContainer();
  QString active_icon_hex = utils::ColorPaletteWrapper::Argb2Hex(icon_color);
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QWidget[class='app_drawer_side_pane_item'] {
        min-height: 48px;
        max-height: 48px;
        background: transparent;
        border: 0px solid transparent;
        border-radius: 24px;
      }
        
      QWidget[class='app_drawer_side_pane_item_active'] {
        min-height: 48px;
        max-height: 48px;
        background: %t1%;
        border: 0px solid %t1%;
        border-radius: 24px;
      }
        
      QLabel[class='app_drawer_side_pane_item_icon'] {
        color: %t2%;
        font-size: 24px;
      }
        
      QLabel[class='app_drawer_side_pane_item_icon_active'] {
        color: %t3%;
        font-size: 24px;
      }
        
      QLabel[class='app_drawer_side_pane_item_label'] {
        color: %t2%;
        font-size: 16px;
      }
        
      QLabel[class='app_drawer_side_pane_item_label_active'] {
        color: %t3%;
        font-size: 16px;
      })"""), QList<QString>{bg_hex, icon_hex, active_icon_hex});
}

static QString GetAppDrawerStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  return GetAppDrawerBackground(palette)
    + GetAppDarwerAppBarBaseStyle(palette)
    + GetAppDarwerToolBarStyle(palette)
    + GetAppDarwerSidePaneItemStyle(palette);
}

}  // namespace loader
}  // namespace panel

#endif  // SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_STYLE_H_
