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

#include <QGuiApplication>
#include <QWindow>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/utils/layer_shell_helper/layer_shell_helper.h"
#include "src/utils/layer_shell_helper/xorg_panel_helper.h"

#ifdef HUSKY_USE_VENDORED_LAYERSHELLQT
#include "lib/3rdparty/layer-shell-qt/src/interfaces/window.h"
#else
#include <LayerShellQt/window.h>
#endif

namespace panel {
namespace utils {

LayerShellHelper::LayerShellHelper(QObject* parent) : QObject(parent) {
  LOG(INFO) << absl::StrCat("LayerShellHelper constructer invoked!!");
}

// Sets the input/visual mask to only the bar strip.
// Call this after the drawer finishes closing.
void LayerShellHelper::SetBarOnlyMask(QObject* window_obj, int bar_height) {
  QWindow* window = qobject_cast<QWindow*>(window_obj);
  if (window == nullptr) {
    return;
  }

  window->setMask(QRegion(0, 0, window->width(), bar_height));
}

// Sets the input/visual mask to bar + the open drawer rectangle.
// drawer_x is relative to the window's left edge.
void LayerShellHelper::SetBarAndDrawerMask(QObject* window_obj, int bar_height,
    int drawer_x, int drawer_w, int drawer_h) {
  QWindow* window = qobject_cast<QWindow*>(window_obj);
  if (window == nullptr) {
    return;
  }

  QRegion mask(0, 0, window->width(), bar_height);
  mask |= QRegion(drawer_x, bar_height, drawer_w, drawer_h);  // Bitwise OR
  window->setMask(mask);
}

// Sets the input mask to the full window area so clicks anywhere in the
// window (including transparent regions outside the drawer) are captured.
// Used when the drawer is open to enable click-outside-to-dismiss.
void LayerShellHelper::SetFullMask(QObject* window_obj) {
  QWindow* window = qobject_cast<QWindow*>(window_obj);
  if (window == nullptr) {
    return;
  }

  window->setMask(QRegion(0, 0, window->width(), window->height()));

  // On X11, _NET_WM_WINDOW_TYPE_DOCK windows are not focused by the WM.
  // requestActivate() sends _NET_ACTIVE_WINDOW which GXDE might ignores for
  // DOCK windows. xcb_set_input_focus bypasses WM focus policy directly.
  if (QGuiApplication::platformName() == QLatin1String("xcb")) {
    panel::utils::XOrgPanelHelper::RequestXorgFocus(window);
  }
}

// Configures a window as a full-screen overlay surface.
// On Wayland: uses LayerShell overlay layer.
// On X11: adds WindowStaysOnTopHint so the dialog floats above the dock.
void LayerShellHelper::SetupOverlayWindow(QObject* window_obj) {
  QWindow* window = qobject_cast<QWindow*>(window_obj);
  if (window == nullptr) {
    return;
  }

  if (QGuiApplication::platformName() == QLatin1String("xcb")) {
    panel::utils::XOrgPanelHelper::SetupXorgOverlayWindow(window);
    return;
  }

  LayerShellQt::Window* layer_shell = LayerShellQt::Window::get(window);
  layer_shell->setAnchors(static_cast<LayerShellQt::Window::Anchor>(
    LayerShellQt::Window::Anchor::AnchorTop |
    LayerShellQt::Window::Anchor::AnchorBottom |
    LayerShellQt::Window::Anchor::AnchorLeft |
    LayerShellQt::Window::Anchor::AnchorRight));
  layer_shell->setLayer(LayerShellQt::Window::LayerOverlay);
  layer_shell->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityOnDemand);
  layer_shell->setExclusiveZone(-1);
}

void LayerShellHelper::setBarOnlyMask(QObject* window_obj, int bar_height) {
  SetBarAndDrawerMask(window_obj, bar_height, 0, 0, 0);
}

void LayerShellHelper::setBarAndDrawerMask(QObject* window_obj, int bar_height,
    int drawer_x, int drawer_w, int drawer_h) {
  SetBarAndDrawerMask(window_obj, bar_height, drawer_x, drawer_w, drawer_h);
}

void LayerShellHelper::setFullMask(QObject* window_obj) {
  SetFullMask(window_obj);
}

void LayerShellHelper::setupOverlayWindow(QObject* window_obj) {
  SetupOverlayWindow(window_obj);
}

}  // namespace utils
}  // namespace panel
