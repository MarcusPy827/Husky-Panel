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

#ifndef SRC_THEME_LOADER_QUICK_THEME_PROVIDER_H_
#define SRC_THEME_LOADER_QUICK_THEME_PROVIDER_H_

#include <QObject>
#include <QColor>

#include "lib/3rdparty/material-color-utilities/cpp/dynamiccolor/dynamic_scheme.h"

namespace panel {
namespace loader {

class QuickThemeProvider : public QObject {
  Q_OBJECT

  Q_PROPERTY(QColor status_bar_bg
    READ GetStatusBarBg
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor surface_container
    READ GetSurfaceContainer
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor on_surface
    READ GetOnSurface
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor state_layer_hover
    READ GetStateLayerHover
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor state_layer_pressed
    READ GetStateLayerPressed
    NOTIFY SchemeUpdated)

 public:
  explicit QuickThemeProvider(QObject* parent = nullptr);
  ~QuickThemeProvider();

  void RefreshTheme();
  const QColor GetStatusBarBg();
  const QColor GetSurfaceContainer();
  const QColor GetOnSurface();
  const QColor GetStateLayerHover();
  const QColor GetStateLayerPressed();

 private:
  QColor status_bar_bg_;
  QColor surface_container_;
  QColor on_surface_;
  QColor state_layer_hover_;
  QColor state_layer_pressed_;

  void WriteScheme(const material_color_utilities::DynamicScheme & scheme);
  int WriteSchemeForStatusBar(
    const material_color_utilities::DynamicScheme & scheme);

 signals:
  void SchemeUpdated();
};

}  // namespace loader
}  // namespace panel

#endif  // SRC_THEME_LOADER_QUICK_THEME_PROVIDER_H_
