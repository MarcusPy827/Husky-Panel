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

#ifndef SRC_COMPONENTS_CALENDAR_CALENDAR_STYLE_H_
#define SRC_COMPONENTS_CALENDAR_CALENDAR_STYLE_H_

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

static QString GetCalendarBackground(
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

static QString GetCalendarTitleBar(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb bg_color = palette->GetSurfaceContainer();
  QString bg_hex = utils::ColorPaletteWrapper::Argb2Hex(bg_color);
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QWidget[class='calendar_title_bar'] {
        background: %t1%;
        border: 0px solid %t1%;
        border-top-left-radius: 12px;
        border-top-right-radius: 12px;
        border-bottom-left-radius: 0px;
        border-bottom-right-radius: 0px;
        min-height: 32px;
        max-height: 32px;
      })"""), QList<QString>{bg_hex});
}

static QString GetCalendarTitle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb text_color = palette->GetOnSurface();
  QString text_hex = utils::ColorPaletteWrapper::Argb2Hex(text_color);
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QWidget[class='calendar_window_title'] {
        color: %t1%;
        background: transparent;
        font-size: 16px;
        font-weight: bold;
      })"""), QList<QString>{text_hex});
}

static QString GetCalendarContent(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb bg_color = palette->GetSurfaceContainer();
  QString bg_hex = utils::ColorPaletteWrapper::Argb2Hex(bg_color);
  material_color_utilities::Argb text_color = palette->GetOnSurface();
  QString text_hex = utils::ColorPaletteWrapper::Argb2Hex(text_color);
  QString text_dim = utils::Colors::ApplyOpacityToHexColor(text_hex, 0.8);
  material_color_utilities::Argb state_layer_color = palette
    ->GetOnSurfaceVariant();
  QString state_layer_hex_raw = utils::ColorPaletteWrapper::Argb2Hex(
    state_layer_color, true);
  QString state_layer_hex_hover = utils::Colors::ApplyOpacityToHexColor(
    state_layer_hex_raw, 0.08);
  QString state_layer_hex_pressed = utils::Colors::ApplyOpacityToHexColor(
    state_layer_hex_raw, 0.1);
  material_color_utilities::Argb on_primary_color = palette->GetOnPrimary();
  QString on_primary_hex = utils::ColorPaletteWrapper::Argb2Hex(
    on_primary_color);
  material_color_utilities::Argb primary_color = palette->GetPrimary();
  QString primary_hex = utils::ColorPaletteWrapper::Argb2Hex(primary_color);
  material_color_utilities::Argb norm_bg_color = palette->GetBackground();
  QString norm_bg_hex = utils::ColorPaletteWrapper::Argb2Hex(norm_bg_color);
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QCalendarWidget[class='calendar_w'] {
        border: 0px solid %t1%;
        border-top-left-radius: 0px;
        border-top-right-radius: 0px;
        border-bottom-left-radius: 12px;
        border-bottom-right-radius: 12px;
        outline: none;
      }

      QCalendarWidget[class='calendar_w'] QWidget#qt_calendar_navigationbar {
        color: %t2%;
        background: %t1%;
        font-size: 16px;
      }

      QCalendarWidget[class='calendar_w'] QToolButton::menu-indicator {
        image: none; 
        width: 0px;
      }

      QCalendarWidget[class='calendar_w'] QAbstractItemView:enabled {
        background: %t8%;
        border: 0px solid %t1%;
        border-top-left-radius: 0px;
        border-top-right-radius: 0px;
        border-bottom-left-radius: 12px;
        border-bottom-right-radius: 12px;
        outline: none;
      }

      QCalendarWidget QHeaderView::section {
        background-color: %t8%;
        background: %t8%;
        color: %t2%;
        padding: 4px;
        border: none;
        font-weight: bold;
        font-size: 16px;
      }

      QCalendarWidget[class='calendar_w'] QTableView {
        background-color: %t8%;
        selection-color: %t6%;
        selection-background-color: %t7%;
        outline: none;
        border: 0px solid %t2%;
        border-top-left-radius: 0px;
        border-top-right-radius: 0px;
        border-bottom-left-radius: 12px;
        border-bottom-right-radius: 12px;
      }
        
      QToolButton[class='calendar_handle_month_btn'] {
        color: %t2%;
        background: transparent;
        border: 0px solid %t3%;
        border-radius: 6px;
        font-size: 18px;
        height: 32px;
        width: 24px;
        padding-left: 8px;
        padding-right: 8px;
      }
        
      QToolButton[class='calendar_handle_month_btn']:hover {
        color: %t2%;
        background: %t3%;
        border: 0px solid %t3%;
        border-radius: 6px;
        font-size: 18px;
        height: 32px;
        padding-left: 8px;
        padding-right: 8px;
      }
        
      QToolButton[class='calendar_handle_month_btn']:pressed {
        color: %t2%;
        background: %t4%;
        border: 0px solid %t4%;
        border-radius: 6px;
        font-size: 18px;
        height: 32px;
        padding-left: 8px;
        padding-right: 8px;
      }
        
      QToolButton[class='calendar_handle_time_btn'] {
        color: %t2%;
        background: transparent;
        border: 0px solid %t3%;
        border-radius: 6px;
        font-size: 16px;
        height: 32px;
        padding-left: 8px;
        padding-right: 8px;
      }
        
      QToolButton[class='calendar_handle_time_btn']:hover {
        color: %t2%;
        background: %t3%;
        border: 0px solid %t3%;
        border-radius: 6px;
        font-size: 16px;
        height: 32px;
        padding-left: 8px;
        padding-right: 8px;
      }
        
      QToolButton[class='calendar_handle_time_btn']:pressed {
        color: %t2%;
        background: %t4%;
        border: 0px solid %t4%;
        border-radius: 6px;
        font-size: 16px;
        height: 32px;
        padding-left: 8px;
        padding-right: 8px;
      }
        
      *[class='cal_header'] {
        background: %t8% !important;
      })"""), QList<QString>{bg_hex, text_hex, state_layer_hex_hover,
                             state_layer_hex_pressed, text_dim, on_primary_hex,
                             primary_hex, norm_bg_hex});
}

static QString GetCalendarStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  return GetCalendarBackground(palette)
    + GetCalendarTitleBar(palette)
    + GetCalendarTitle(palette)
    + GetCalendarContent(palette);
}

}  // namespace loader
}  // namespace panel

#endif  // SRC_COMPONENTS_CALENDAR_CALENDAR_STYLE_H_
