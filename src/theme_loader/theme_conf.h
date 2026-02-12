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
#include "lib/3rdparty/material-color-utilities/cpp/dynamiccolor/dynamic_scheme.h"

namespace flake {
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
      *[class='background_layer'] {
        background: %t1%;
        border: 1px solid %t2%;
        border-radius: 8px;
      }
        
      *[class='background_layer_maximized'] {
        background: %t1%;
        border: 0px solid %t2%;
        border-radius: 0px;
      })"""), QList<QString>{bg_hex, border_hex});
}

static QString GetTitleBarStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      *[class='title_bar'] {
        min-height: 32px;
        max-height: 32px;
        background: transparent;
      })"""), QList<QString>{});
}

static QString GetWindowTitleStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb text_color = palette->GetPrimary();
  QString text_hex = utils::ColorPaletteWrapper::Argb2Hex(text_color,
    true);

  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      *[class='window_title'] {
        color: %t1%;
        font-size: 16px;
      })"""), QList<QString>{text_hex});
}

static QString GetTitleBarMenuBtnStyle(
  const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb icon_color = palette->GetPrimary();
  QString icon_hex = utils::ColorPaletteWrapper::Argb2Hex(icon_color, true);
  material_color_utilities::Argb border_color = palette->GetSurfaceContainer();
  QString border_hex = utils::ColorPaletteWrapper::Argb2Hex(border_color,
    true);
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
      *[class='title_bar_menu_item'] {
        color: %t1%;
        background: transparent;
        border: 1px solid %t2%;
        border-radius: 4px;
        height: 24px;
        outline: none;
        padding-left: 4px;
        padding-right: 4px;
        font-size: 16px;
      }
          
      *[class='title_bar_menu_item']:hover {
        color: %t1%;
        background: %t3%;
        border: 1px solid %t2%;
        border-radius: 4px;
        height: 24px;
        outline: none;
        padding-left: 4px;
        padding-right: 4px;
        font-size: 16px;
      }
          
      *[class='title_bar_menu_item']:pressed {
        color: %t1%;
        background: %t4%;
        border: 1px solid %t2%;
        border-radius: 4px;
        height: 24px;
        outline: none;
        padding-left: 4px;
        padding-right: 4px;
        font-size: 16px;
      })"""), QList<QString>{icon_hex, border_hex, state_layer_hex_hover,
                             state_layer_hex_pressed});
}

static QString GetTitleBarActionBtnStyle(
  const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb icon_color = palette->GetPrimary();
  QString icon_hex = utils::ColorPaletteWrapper::Argb2Hex(icon_color, true);
  material_color_utilities::Argb border_color = palette->GetSurfaceContainer();
  QString border_hex = utils::ColorPaletteWrapper::Argb2Hex(border_color,
    true);
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
      *[class='title_bar_window_action_btn'] {
        color: %t1%;
        background: transparent;
        border: 1px solid %t2%;
        border-radius: 4px;
        height: 24px;
        width: 24px;
        outline: none;
        font-size: 18px;
      }
          
      *[class='title_bar_window_action_btn']:hover {
        color: %t1%;
        background: %t3%;
        border: 1px solid %t2%;
        border-radius: 4px;
        height: 24px;
        width: 24px;
        outline: none;
        font-size: 18px;
      }
          
      *[class='title_bar_window_action_btn']:pressed {
        color: %t1%;
        background: %t4%;
        border: 1px solid %t2%;
        border-radius: 4px;
        height: 24px;
        width: 24px;
        outline: none;
        font-size: 18px;
      })"""), QList<QString>{icon_hex, border_hex, state_layer_hex_hover,
                             state_layer_hex_pressed});
}

static QString GetTitleBarCloseStyle(
  const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb icon_color = palette->GetPrimary();
  QString icon_hex = utils::ColorPaletteWrapper::Argb2Hex(icon_color, true);
  material_color_utilities::Argb border_color = palette->GetSurfaceContainer();
  QString border_hex = utils::ColorPaletteWrapper::Argb2Hex(border_color,
    true);
  material_color_utilities::Argb state_layer_color = palette
    ->GetError();
  QString state_layer_hex_raw = utils::ColorPaletteWrapper::Argb2Hex(
    state_layer_color, true);
  QString state_layer_hex_hover = utils::Colors::ApplyOpacityToHexColor(
    state_layer_hex_raw, 0.08);
  material_color_utilities::Argb icon_hover_color = palette->GetError();
  QString icon_hover_hex = utils::ColorPaletteWrapper::Argb2Hex(
    icon_hover_color, true);
  QString state_layer_hex_pressed = state_layer_hex_raw;
  material_color_utilities::Argb icon_pressed_color = palette->GetOnError();
  QString icon_pressed_hex = utils::ColorPaletteWrapper::Argb2Hex(
    icon_pressed_color, true);

  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      *[class='title_bar_window_close_btn'] {
        color: %t1%;
        background: transparent;
        border: 1px solid %t2%;
        border-radius: 4px;
        height: 24px;
        width: 24px;
        outline: none;
        font-size: 18px;
      }
          
      *[class='title_bar_window_close_btn']:hover {
        color: %t4%;
        background: %t3%;
        border: 1px solid %t2%;
        border-radius: 4px;
        height: 24px;
        width: 24px;
        outline: none;
        font-size: 18px;
      }
          
      *[class='title_bar_window_close_btn']:pressed {
        color: %t5%;
        background: %t6%;
        border: 1px solid %t2%;
        border-radius: 4px;
        height: 24px;
        width: 24px;
        outline: none;
        font-size: 18px;
      })"""), QList<QString>{icon_hex, border_hex, state_layer_hex_hover,
                             icon_hover_hex, icon_pressed_hex,
                             state_layer_hex_pressed});
}

static QString GetNavRailStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb bg_color = palette->GetSurfaceContainer();
  QString bg_hex = utils::ColorPaletteWrapper::Argb2Hex(bg_color,
    true);

  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      *[class='nav_rail'] {
        min-width: 80px;
        max-width: 80px;
        background: %t1%;
      })"""), QList<QString>{bg_hex});
}

static QString GetAppBarStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb bg_color = palette->GetSurfaceContainer();
  QString bg_hex = utils::ColorPaletteWrapper::Argb2Hex(bg_color,
    true);
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QWidget[class='app_bar'] {
        min-height: 64px;
        max-height: 64px;
        background: %t1%;
        border: none;
        margin: 0px;
        padding: 0px;
      })"""), QList<QString>{bg_hex});
}

static QString GetAppBarIconBtnStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb border_color = palette->GetSurfaceContainer();
  QString border_hex = utils::ColorPaletteWrapper::Argb2Hex(border_color,
    true);
  material_color_utilities::Argb state_layer_color = palette->GetOnSurface();
  QString state_layer_hex_raw = utils::ColorPaletteWrapper::Argb2Hex(
    state_layer_color, true);
  QString state_layer_hex_hover = utils::Colors::ApplyOpacityToHexColor(
    state_layer_hex_raw, 0.08);
  QString state_layer_hex_pressed = utils::Colors::ApplyOpacityToHexColor(
    state_layer_hex_raw, 0.1);
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QPushButton[class='app_bar_icon_btn'] {
        color: %t1%;
        background: transparent;
        height: 48px;
        width: 48px;
        border: 0px solid %t2%;
        border-radius: 24px;
        font-size: 24px;
        outline: none;
      }
        
      QPushButton[class='app_bar_icon_btn']:hover {
        background: %t3%;
      }
        
      QPushButton[class='app_bar_icon_btn']:pressed {
        background: %t4%;
      })"""), QList<QString>{state_layer_hex_raw, border_hex,
                             state_layer_hex_hover, state_layer_hex_pressed});
}

static QString GetAppBarSearchBarStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb text_color = palette->GetOnSurfaceVariant();
  QString text_hex = utils::ColorPaletteWrapper::Argb2Hex(text_color, true);
  material_color_utilities::Argb bg_color = palette
    ->GetSurfaceContainerLowest();
  QString bg_hex = utils::ColorPaletteWrapper::Argb2Hex(bg_color, true);
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QLineEdit[class='app_bar_search_bar'] {
        color: %t1%;
        background: %t2%;
        border: 0px solid %t2%;
        border-radius: 24px;
        height: 48px;
        padding-left: 24px;
        padding-right: 24px;
        font-size: 16px;
      })"""), QList<QString>{text_hex, bg_hex});
}

static QString GetFabStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  material_color_utilities::Argb text_color = palette->GetOnPrimaryContainer();
  QString text_hex = utils::ColorPaletteWrapper::Argb2Hex(text_color, true);
  material_color_utilities::Argb bg_color = palette
    ->GetPrimaryContainer();
  QString bg_hex = utils::ColorPaletteWrapper::Argb2Hex(bg_color, true);
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
      QPushButton[class='fab'] {
        min-width: 48px;
        max-width: 48px;
        min-height: 48px;
        max-height: 48px;
        color: %t1%;
        background: %t2%;
        border: 0px solid %t2%;
        border-radius: 16px;
        font-size: 24px;
        outline: none;
      }
        
      QPushButton[class='fab']:hover {
        min-width: 48px;
        max-width: 48px;
        min-height: 48px;
        max-height: 48px;
        color: %t1%;
        background: %t3%;
        border: 0px solid %t3%;
        border-radius: 16px;
        font-size: 24px;
        outline: none;
      }
        
      QPushButton[class='fab']:pressed {
        min-width: 48px;
        max-width: 48px;
        min-height: 48px;
        max-height: 48px;
        color: %t1%;
        background: %t4%;
        border: 0px solid %t4%;
        border-radius: 16px;
        font-size: 24px;
        outline: none;
      })"""), QList<QString>{text_hex, bg_hex, state_layer_hex_hover,
                             state_layer_hex_pressed});
}

static QString GetSideBarStyle(
    const std::unique_ptr<material_color_utilities::DynamicScheme>& palette) {
  return utils::Strings::TemplateCat(
    QStringLiteral(R"""(
      QWidget[class='side_bar'] {
        min-width: 180px;
        max-width: 180px;
      })"""), QList<QString>{});
}

static StyleTuple GetThemeMap(const SchemeTuple& conf) {
  return {
    GetAppBackground(conf.light)
      + GetTitleBarStyle(conf.light)
      + GetWindowTitleStyle(conf.light)
      + GetTitleBarMenuBtnStyle(conf.light)
      + GetTitleBarActionBtnStyle(conf.light)
      + GetTitleBarCloseStyle(conf.light)
      + GetNavRailStyle(conf.light)
      + GetAppBarStyle(conf.light)
      + GetAppBarIconBtnStyle(conf.light)
      + GetAppBarSearchBarStyle(conf.light)
      + GetFabStyle(conf.light)
      + GetSideBarStyle(conf.light),
    GetAppBackground(conf.dark)
      + GetTitleBarStyle(conf.dark)
      + GetWindowTitleStyle(conf.dark)
      + GetTitleBarMenuBtnStyle(conf.dark)
      + GetTitleBarActionBtnStyle(conf.dark)
      + GetTitleBarCloseStyle(conf.dark)
      + GetNavRailStyle(conf.dark)
      + GetAppBarStyle(conf.dark)
      + GetAppBarIconBtnStyle(conf.dark)
      + GetAppBarSearchBarStyle(conf.dark)
      + GetFabStyle(conf.dark)
      + GetSideBarStyle(conf.dark)
  };
}

}  // namespace loader
}  // namespace flake

#endif  // SRC_THEME_LOADER_THEME_CONF_H_
