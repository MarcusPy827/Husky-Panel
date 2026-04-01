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
  Q_PROPERTY(QColor status_bar_primary
    READ GetStatusBarPrimary
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor status_bar_surface_fg
    READ GetStatusBarSurfaceFg
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor status_bar_state_layer_hover
    READ GetStatusBarStateLayerHover
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor status_bar_state_layer_pressed
    READ GetStatusBarStateLayerPressed
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor primary
    READ GetPrimary
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor surface_container
    READ GetSurfaceContainer
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor surface_fg
    READ GetSurfaceFg
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor state_layer_hover
    READ GetStateLayerHover
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor state_layer_pressed
    READ GetStateLayerPressed
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor surface_bg
    READ GetSurfaceBg
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor surface_container_low
    READ GetSurfaceContainerLow
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor surface_variant_fg
    READ GetSurfaceVariantFg
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor primary_container
    READ GetPrimaryContainer
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor secondary_container
    READ GetSecondaryContainer
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor secondary_container_fg
    READ GetSecondaryContainerFg
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor surface_container_lowest
    READ GetSurfaceContainerLowest
    NOTIFY SchemeUpdated)
  Q_PROPERTY(QColor surface_container_high
    READ GetSurfaceContainerHigh
    NOTIFY SchemeUpdated)

 public:
  explicit QuickThemeProvider(QObject* parent = nullptr);
  ~QuickThemeProvider();

  void RefreshTheme();
  const QColor GetStatusBarBg();
  const QColor GetStatusBarPrimary();
  const QColor GetStatusBarSurfaceFg();
  const QColor GetStatusBarStateLayerHover();
  const QColor GetStatusBarStateLayerPressed();
  const QColor GetPrimary();
  const QColor GetSurfaceContainer();
  const QColor GetSurfaceFg();
  const QColor GetStateLayerHover();
  const QColor GetStateLayerPressed();
  const QColor GetSurfaceBg();
  const QColor GetSurfaceContainerLow();
  const QColor GetSurfaceContainerLowest();
  const QColor GetSurfaceContainerHigh();
  const QColor GetSurfaceVariantFg();
  const QColor GetPrimaryContainer();
  const QColor GetSecondaryContainer();
  const QColor GetSecondaryContainerFg();

 private:
  QColor status_bar_bg_;
  QColor status_bar_primary_;
  QColor status_bar_surface_fg_;
  QColor status_bar_state_layer_hover_;
  QColor status_bar_state_layer_pressed_;
  QColor primary_;
  QColor surface_container_;
  QColor surface_fg_;
  QColor state_layer_hover_;
  QColor state_layer_pressed_;
  QColor surface_bg_;
  QColor surface_container_low_;
  QColor surface_container_lowest_;
  QColor surface_container_high_;
  QColor surface_variant_fg_;
  QColor primary_container_;
  QColor secondary_container_;
  QColor secondary_container_fg_;

  void WriteScheme(const material_color_utilities::DynamicScheme & scheme);
  int WriteSchemeForStatusBar(
    const material_color_utilities::DynamicScheme & scheme);

 signals:
  void SchemeUpdated();
};

}  // namespace loader
}  // namespace panel

#endif  // SRC_THEME_LOADER_QUICK_THEME_PROVIDER_H_
