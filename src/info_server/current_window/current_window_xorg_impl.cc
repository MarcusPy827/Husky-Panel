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

#include <cstring>

#include <KService>
#include <QTimer>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/current_window/current_window_xorg_impl.h"

namespace panel {
namespace backend {

/**
 * @brief Constructs the XOrg/X11 implementation for active window tracking.
 *
 * @param parent (QObject*) The parent QObject.
 * @details Connects to the X display, selects @c PropertyChangeMask on the
 *          root window so that @c _NET_ACTIVE_WINDOW changes are delivered,
 *          then sets up a @c QSocketNotifier on the XCB file descriptor to
 *          receive those events without blocking the Qt event loop.
 *          Finally, reads the initial active window.
 * @return void
 */

CurrentWindowXorgImpl::CurrentWindowXorgImpl(QObject* parent)
    : CurrentWindowProvider(parent),
      conn_(xcb_connect(nullptr, nullptr)),
      root_(XCB_NONE),
      atom_net_active_window_(XCB_NONE),
      atom_wm_class_(XCB_NONE),
      notifier_(nullptr),
      poll_timer_(nullptr),
      last_active_window_(XCB_NONE) {
  if (xcb_connection_has_error(conn_)) {
    LOG(ERROR) << absl::StrCat("CurrentWindowXorgImpl: ",
      "failed to connect to X display.");
    return;
  }

  const xcb_setup_t* setup = xcb_get_setup(conn_);
  root_ = xcb_setup_roots_iterator(setup).data->root;

  atom_net_active_window_ = InternAtom("_NET_ACTIVE_WINDOW");
  atom_wm_class_          = InternAtom("WM_CLASS");

  // Subscribe to PropertyNotify events on the root window so we are notified
  // when _NET_ACTIVE_WINDOW changes.
  const uint32_t mask = XCB_EVENT_MASK_PROPERTY_CHANGE;
  xcb_change_window_attributes(conn_, root_, XCB_CW_EVENT_MASK, &mask);
  xcb_flush(conn_);

  notifier_ = new QSocketNotifier(
    xcb_get_file_descriptor(conn_), QSocketNotifier::Read, this);
  connect(notifier_, &QSocketNotifier::activated,
    this, &CurrentWindowXorgImpl::OnXcbEvent);

  // Fallback poll: some WMs don't send PropertyNotify
  // For every _NET_ACTIVE_WINDOW change. Poll every 500 ms to stay current.
  poll_timer_ = new QTimer(this);
  poll_timer_->setInterval(500);
  connect(poll_timer_, &QTimer::timeout,
    this, &CurrentWindowXorgImpl::UpdateActiveWindow);
  poll_timer_->start();

  UpdateActiveWindow();

  LOG(INFO) << absl::StrCat("CurrentWindowXorgImpl: ",
    "X11 active window tracking is up.");
}


/**
 * @brief Destructor — disconnects from the X display.
 *
 * @return void
 */

CurrentWindowXorgImpl::~CurrentWindowXorgImpl() {
  LOG(INFO) << absl::StrCat("CurrentWindowXorgImpl is being deleted.");
  if (conn_) {
    xcb_disconnect(conn_);
  }
}


/**
 * @brief Interns (looks up or creates) an X atom by name.
 *
 * @param name (const char*) The atom name string.
 * @return (xcb_atom_t) The atom, or @c XCB_NONE on failure.
 */

xcb_atom_t CurrentWindowXorgImpl::InternAtom(const char* name) {
  xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(
    conn_,
    xcb_intern_atom(conn_, 0, static_cast<uint16_t>(strlen(name)), name),
    nullptr);

  if (!reply) {
    return XCB_NONE;
  }

  xcb_atom_t atom = reply->atom;
  free(reply);
  return atom;
}


/**
 * @brief Drains pending XCB events and handles @c XCB_PROPERTY_NOTIFY.
 *
 * @details Called by the @c QSocketNotifier when the X connection fd becomes
 *          readable. Polls all pending events; for each
 *          @c xcb_property_notify_event_t whose atom is
 *          @c _NET_ACTIVE_WINDOW, triggers @c UpdateActiveWindow().
 * @return void
 */

void CurrentWindowXorgImpl::OnXcbEvent() {
  xcb_generic_event_t* event;
  while ((event = xcb_poll_for_event(conn_)) != nullptr) {
    if ((event->response_type & ~0x80) == XCB_PROPERTY_NOTIFY) {
      auto* pn = reinterpret_cast<xcb_property_notify_event_t*>(event);
      if (pn->atom == atom_net_active_window_) {
        UpdateActiveWindow();
      }
    }
    free(event);
  }
}


/**
 * @brief Reads @c _NET_ACTIVE_WINDOW from the root, then resolves the
 *        application name and package name from the focused window's
 *        @c WM_CLASS property.
 *
 * @details Uses @c KService to map the WM_CLASS class name to a friendly
 *          application name. Falls back to the raw class name when no
 *          matching .desktop entry exists. The class name (lowercased) is
 *          used as the package name. Emits @c CurrentWindowChanged() on
 *          success.
 * @return void
 */

void CurrentWindowXorgImpl::UpdateActiveWindow() {
  xcb_window_t active_window = XCB_NONE;

  // Use AnyPropertyType (XCB_ATOM_NONE) so non-standard WMs that store
  // _NET_ACTIVE_WINDOW with a different type are not silently filtered out.
  xcb_get_property_reply_t* reply = xcb_get_property_reply(
    conn_,
    xcb_get_property(conn_, 0, root_, atom_net_active_window_,
      XCB_ATOM_NONE, 0, 1),
    nullptr);

  if (reply && xcb_get_property_value_length(reply) >=
      static_cast<int>(sizeof(xcb_window_t))) {
    active_window =
      *static_cast<xcb_window_t*>(xcb_get_property_value(reply));
  }
  free(reply);

  // Fallback: some compositors (e.g. GXDE/DDE) don't reliably update
  // _NET_ACTIVE_WINDOW; xcb_get_input_focus gives the true focused window.
  if (active_window == XCB_NONE || active_window == root_) {
    xcb_get_input_focus_reply_t* focus =
      xcb_get_input_focus_reply(conn_, xcb_get_input_focus(conn_), nullptr);
    if (focus) {
      active_window = focus->focus;
      free(focus);
    }
  }

  // Walk up the window tree to find the top-level client window, since
  // input focus may land on an internal child widget.
  while (active_window != XCB_NONE && active_window != root_) {
    xcb_query_tree_reply_t* tree = xcb_query_tree_reply(
      conn_, xcb_query_tree(conn_, active_window), nullptr);
    if (!tree || tree->parent == root_) {
      free(tree);
      break;
    }
    xcb_window_t parent = tree->parent;
    free(tree);
    active_window = parent;
  }

  if (active_window == XCB_NONE || active_window == root_ ||
      active_window == XCB_INPUT_FOCUS_POINTER_ROOT) {
    return;
  }

  if (active_window == last_active_window_) {
    return;
  }

  QString wm_class = GetWindowClass(active_window);
  if (wm_class.isEmpty()) {
    return;
  }

  if (wm_class.contains(QLatin1String("HuskyPanel"), Qt::CaseInsensitive)) {
    return;
  }

  last_active_window_ = active_window;
  window_info_.package_name = wm_class.toLower();

  KService::Ptr service = KService::serviceByDesktopName(wm_class);
  if (!service) {
    service = KService::serviceByDesktopName(wm_class.toLower());
  }

  window_info_.application_name = service ? service->name() : wm_class;

  LOG(INFO) << absl::StrCat("CurrentWindowXorgImpl: active window changed — ",
    window_info_.application_name.toStdString());

  emit CurrentWindowChanged();
}


/**
 * @brief Reads the @c WM_CLASS property of the given window and returns the
 *        class name (the second null-terminated string in the property).
 *
 * @param window (xcb_window_t) The window to query.
 * @return (QString) The WM_CLASS class name, or an empty string on failure.
 */

QString CurrentWindowXorgImpl::GetWindowClass(xcb_window_t window) {
  xcb_get_property_reply_t* reply = xcb_get_property_reply(
    conn_,
    xcb_get_property(conn_, 0, window, atom_wm_class_,
      XCB_ATOM_NONE, 0, 256),
    nullptr);

  if (!reply || reply->type == XCB_ATOM_NONE ||
      xcb_get_property_value_length(reply) == 0) {
    free(reply);
    return {};
  }

  auto* data = static_cast<const char*>(xcb_get_property_value(reply));
  int len    = xcb_get_property_value_length(reply);

  // WM_CLASS is two consecutive null-terminated strings: "instance\0class\0"
  // We want the class name (the second string).
  int first_len = static_cast<int>(strnlen(data, len));
  QString result;
  if (first_len + 1 < len) {
    result = QString::fromUtf8(data + first_len + 1);
  } else if (first_len > 0) {
    // Malformed: only one string present; use it as fallback.
    result = QString::fromUtf8(data, first_len);
  }

  free(reply);
  return result;
}


/**
 * @brief Returns the friendly application name of the current active window.
 *
 * @return (QString) The application name.
 */

QString CurrentWindowXorgImpl::GetApplicationName() {
  return window_info_.application_name;
}


/**
 * @brief Returns the package name (lowercased WM_CLASS) of the current active
 *        window.
 *
 * @return (QString) The package name.
 */

QString CurrentWindowXorgImpl::GetPackageName() {
  return window_info_.package_name;
}

}  // namespace backend
}  // namespace panel
