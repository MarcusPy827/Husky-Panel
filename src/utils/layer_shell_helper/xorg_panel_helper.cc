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

#include "src/utils/layer_shell_helper/xorg_panel_helper.h"

#include <cstdlib>
#include <cstring>

#include <QGuiApplication>
#include <QScreen>
#include <QWindow>

#if QT_CONFIG(xcb)
#include <xcb/xcb.h>
#endif

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"

namespace panel {
namespace utils {

#if QT_CONFIG(xcb)
/**
 * @brief Interns an X11 atom by name.
 * 
 * @note This function returns @c XCB_ATOM_NONE on failure.
 * @param conn (xcb_connection_t*) The XCB connection to used.
 * @param name (const char*) The name of the atom
 * @return (xcb_atom_t) The interned atom. 
 */

static xcb_atom_t InternXcbAtom(xcb_connection_t* conn, const char* name) {
  xcb_intern_atom_cookie_t cookie =
    xcb_intern_atom(conn, 0, static_cast<uint16_t>(strlen(name)), name);
  xcb_intern_atom_reply_t* reply =
    xcb_intern_atom_reply(conn, cookie, nullptr);
  xcb_atom_t atom = (reply != nullptr) ? reply->atom : XCB_ATOM_NONE;

  free(reply);
  return atom;
}
#endif  // QT_CONFIG(xcb)


/**
 * @brief Sets up the status bar for the X11/Xorg session.
 * 
 * @details This function sets the frameless window hint for the target status
 *          bar window and anchors it to top-left of primary screen. After
 *          showing the window, it sets the @c _NET_WM_WINDOW_TYPE_DOCK and
 *          @c _NET_WM_STRUT_PARTIAL to reserve the space needed.
 * @note This function also ensures that the status bar is avaliable on all the
 *       virtual desktop.
 * @param window (QWindow*) The target panel.
 * @param bar_height (int) The height of the status bar.
 * @return void.
 */

void XOrgPanelHelper::SetupXorgPanelWindow(QWindow* window, int bar_height) {
  if (window == nullptr) {
    LOG(ERROR) << absl::StrCat("Got null status bar pointer, aborting ",
      "setting up status bar for X11/Xorg session...");
    return;
  }

  QScreen* screen = QGuiApplication::primaryScreen();
  if (screen == nullptr) {
    LOG(ERROR) << absl::StrCat("Could NOT get primary screen, aborting...");
    return;
  }

  // Anchors top-left of the primary screen while QML charges for width/height
  // to screen dimensions.
  window->setPosition(screen->geometry().topLeft());
  window->setFlag(Qt::FramelessWindowHint);
  window->show();
  LOG(INFO) << absl::StrCat("X11 panel window shown at (",
    screen->geometry().x(), ", ", screen->geometry().y(), ").");

#if QT_CONFIG(xcb)
  // Open an independent XCB connection to the same X server.
  // Qt's connection is not exposed via public headers on all distros, but
  // window properties are stored by the X server on the window object, so any
  // connection targeting the correct window ID works fine.
  xcb_connection_t* conn = xcb_connect(getenv("DISPLAY"), nullptr);
  if (xcb_connection_has_error(conn)) {
    LOG(ERROR) << absl::StrCat("Failed to connect to X server.");
    xcb_disconnect(conn);
    return;
  }

  xcb_window_t xwin = static_cast<xcb_window_t>(window->winId());

  xcb_atom_t net_wm_window_type = InternXcbAtom(conn, "_NET_WM_WINDOW_TYPE");
  xcb_atom_t net_wm_window_type_dock = InternXcbAtom(conn,
    "_NET_WM_WINDOW_TYPE_DOCK");
  xcb_atom_t net_wm_strut = InternXcbAtom(conn, "_NET_WM_STRUT");
  xcb_atom_t net_wm_strut_partial = InternXcbAtom(conn,
    "_NET_WM_STRUT_PARTIAL");
  xcb_atom_t net_wm_state = InternXcbAtom(conn, "_NET_WM_STATE");
  xcb_atom_t net_wm_state_sticky = InternXcbAtom(conn, "_NET_WM_STATE_STICKY");

  // Declare this window as a dock
  xcb_change_property(conn, XCB_PROP_MODE_REPLACE, xwin,
    net_wm_window_type, XCB_ATOM_ATOM, 32, 1, &net_wm_window_type_dock);

  // Legacy strut: left = 0; right = 0; top = bar_height; bottom = 0.
  uint32_t strut[4] = {
    0,
    0,
    static_cast<uint32_t>(bar_height),
    0
  };

  xcb_change_property(conn, XCB_PROP_MODE_REPLACE, xwin,
    net_wm_strut, XCB_ATOM_CARDINAL, 32, 4, strut);

  // Extended strut: reserve bar_height px along the top edge of this screen.
  const uint32_t screen_x = static_cast<uint32_t>(screen->geometry().x());
  const uint32_t screen_width = static_cast<uint32_t>(screen->geometry()
    .width());
  const uint32_t bar_height_casted = static_cast<uint32_t>(bar_height);

  uint32_t strut_partial[12] = {
    0, 0,  // left, right
    bar_height_casted, 0,  // top, bottom
    0, 0,  // left_start_y, left_end_y
    0, 0,  // right_start_y, right_end_y
    screen_x, screen_x + screen_width - 1,  // top_start_x, top_end_x
    0, 0,  // bottom_start_x, bottom_end_x
  };

  xcb_change_property(conn, XCB_PROP_MODE_REPLACE, xwin,
    net_wm_strut_partial, XCB_ATOM_CARDINAL, 32, 12, strut_partial);

  // Make the panel visible on all virtual desktops.
  xcb_change_property(conn, XCB_PROP_MODE_REPLACE, xwin,
    net_wm_state, XCB_ATOM_ATOM, 32, 1, &net_wm_state_sticky);

  xcb_flush(conn);
  LOG(INFO) << absl::StrCat("Xorg DOCK/STRUT properties has been set at: ",
    "top = ", bar_height, "px.");
#else
  LOG(ERROR) << absl::StrCat("XCB unavaliable, failed to set X11 status ",
    "bar...");
#endif  // QT_CONFIG(xcb)
}


/**
 * @brief This function configures an overlay window for X11/Xorg window.
 * 
 * @note This function adds @c Qt::WindowStaysOnTopHint so that the overlay/
 *       flyout could be fixed at top.
 * @param window (QWindow*) The target overlay/flyout window.
 * @return void.
 */

void XOrgPanelHelper::SetupXorgOverlayWindow(QWindow* window) {
  if (window == nullptr) {
    LOG(ERROR) << absl::StrCat("Got null flyout pointer, aborting...");
    return;
  }

  window->setFlag(Qt::WindowStaysOnTopHint);
  LOG(INFO) << absl::StrCat("Successfully set window hint for flyout!!");
}


/**
 * @brief Forces keyboard focus to the panel window on X11, bypassing WM focus
 *        policy that normally denies focus to DOCK-type windows.
 *
 * @details DOCK windows are intentionally non-focusable by most WMs. Qt's
 *          requestActivate() sends _NET_ACTIVE_WINDOW which WMs like GXDE
 *          ignore for DOCK windows. xcb_set_input_focus() talks directly to
 *          the X server and bypasses that restriction, allowing text input
 *          in AppDrawer/flyout fields to work correctly.
 * @param window (QWindow*) The panel window that needs keyboard focus.
 * @return void.
 */

void XOrgPanelHelper::RequestXorgFocus(QWindow* window) {
  if (window == nullptr) {
    return;
  }

#if QT_CONFIG(xcb)
  xcb_connection_t* conn = xcb_connect(getenv("DISPLAY"), nullptr);
  if (xcb_connection_has_error(conn)) {
    xcb_disconnect(conn);
    return;
  }

  xcb_window_t xwin = static_cast<xcb_window_t>(window->winId());
  xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT, xwin,
    XCB_CURRENT_TIME);
  xcb_flush(conn);
  xcb_disconnect(conn);

  LOG(INFO) << absl::StrCat("XOrgPanelHelper: forced input focus to panel ",
    "window (xcb_set_input_focus).");
#endif  // QT_CONFIG(xcb)
}

}  // namespace utils
}  // namespace panel
