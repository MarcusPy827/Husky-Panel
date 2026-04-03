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

#include <KService>

#include <cstdint>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/current_window/current_window_wlroots_impl.h"

namespace panel {
namespace backend {

/* ------------------------------------------------------------------------- */
/* Class 'WlrootsHelper' */
/* ------------------------------------------------------------------------- */

/**
 * @brief Constructs a wrapper for the wlr handle.
 * 
 * @param handle (zwlr_foreign_toplevel_handle_v1*) The raw handle.
 * @param parent (QObject*) The parent QObject for this wrapper.
 * @note This wrapper will manage the callback of title/app_id/state/done/closed,
 *       and emit Activated signals accordingly.
 * @return void
 */

WlrootsHelper::WlrootsHelper(struct ::zwlr_foreign_toplevel_handle_v1* handle,
    QObject* parent) : QObject(parent),
    QtWayland::zwlr_foreign_toplevel_handle_v1(handle) {
  LOG(INFO) << absl::StrCat("Constructor of WlrootsHelper ",
    "has been invoked.");
}


/**
 * @brief The destructor of the wrapper which is responsiblr for cleaning up.
 * 
 * @note We are using @c destory() to release the Wayland resource, then Qt
 *       will take care of the rest of the QObject part.
 * @return void
 */

WlrootsHelper::~WlrootsHelper() {
  LOG(INFO) << absl::StrCat("Destructor of WlrootsHelper ",
    "has been invoked.");
  destroy();
}


/**
 * @brief Handle the @c title event of foreign_toplevel_handle
 * 
 * @param title (QString &) The window title.
 * @details The title is ONLY cached to @c pending_title_ and it would take
 *          effect while @c done() is called.
 * @note Overridding the virtual function from
 *       @c zwlr_foreign_toplevel_handle_v1.
 * @return void
 */

void WlrootsHelper::zwlr_foreign_toplevel_handle_v1_title(
    const QString& title) {
  pending_title_ = title;
}


/**
 * @brief Handle the @c app_id event of @c foreign_toplevel_handle.
 * 
 * @param app_id (QString &) The application id, which is usually the
 *                           @c .desktop filename without extension name?? Or,
 *                           it could be the @c WM_CLASS for those XWayland
 *                           windows.
 * @details The app_id is ONLY cached to @c pending_app_id_ and it would take
 *          effects while @c done() is called.
 * @note Overridding the virtual function from
 *       @c zwlr_foreign_toplevel_handle_v1.
 * 
 * @return void
 */

void WlrootsHelper::zwlr_foreign_toplevel_handle_v1_app_id(
    const QString& app_id) {
  pending_app_id_ = app_id;
}


/**
 * @brief Handle the @c state event of @c foreign_toplevel_handle.
 * 
 * @param state (wl_array*) The state array, consisting of a list of state
 *                          enums.
 * @details Parse whether state contains @c state_activated, and writes it to
 *          @c pending_activated_. It would decide whether to emit @c Activated
 *          while @c done() is called.
 * @note Overridding the virtual function from
 *       @c zwlr_foreign_toplevel_handle_v1.
 * @return void
 */

void WlrootsHelper::zwlr_foreign_toplevel_handle_v1_state(wl_array* state) {
  pending_activated_ = false;
  const uint32_t kActivated =
    QtWayland::zwlr_foreign_toplevel_handle_v1::state::state_activated;

  auto* data = static_cast<uint32_t*> (state->data);
  size_t count = state->size / sizeof(uint32_t);

  for (size_t i = 0; i < count; i++) {
    if (data[i] == kActivated) {
      pending_activated_ = true;
      break;
    }
  }
}


/**
 * @brief Handle the @c done event of @c foreign_toplevel_handle.
 * 
 * @details This represents the end of a batch update. If @c pending_activated_
 *          is true, then the signal 
 *          @c Activated(pending_app_id_, @c pending_title_) would be emitted.
 * @note Overridding the virtual function from
 *       @c zwlr_foreign_toplevel_handle_v1.
 * @return void
 */

void WlrootsHelper::zwlr_foreign_toplevel_handle_v1_done() {
  if (pending_activated_) {
    emit Activated(pending_app_id_, pending_title_);
  }
}


/**
 * @brief Handle the @c closed event of @c foreign_toplevel_handle.
 * 
 * @details This represents the toplevel window being closed, and we forward
 *          the notification to the manager by emitting 
 *          @c Closed(WlrToplevelHandle *handle).
 * @note Overridding the virtual function from
 *       @c zwlr_foreign_toplevel_handle_v1.
 * @return void
 */

void WlrootsHelper::zwlr_foreign_toplevel_handle_v1_closed() {
  emit Closed(this);
}


/* ------------------------------------------------------------------------- */
/* Class 'WlrToplevelMgr' */
/* ------------------------------------------------------------------------- */

/**
 * @brief Constructor of WlrToplevelMgr.
 * 
 * @details Constructs a wrapper for zwlr_foreign_toplevel_manager_v1, which is
 *          responsible for managing the toplevel windows.
 * @note The binding is using a requested version of 3.
 * @note Whether the binding is successful can be checked via @c isActive().
 * @return void
 */

WlrToplevelMgr::WlrToplevelMgr()
    : QWaylandClientExtensionTemplate<WlrToplevelMgr>(3) {
  LOG(INFO) << absl::StrCat("Constructor of WlrToplevelMgr ",
    "has been invoked!!");
}


/**
 * @brief Destructor of WlrToplevelMgr.
 * 
 * @details If the extension is STILL active, call @c stop() to disconnect from
 *          the global object. Then Qt will take care of the rest of the
 *          QObject part.
 * @return void
 */

WlrToplevelMgr::~WlrToplevelMgr() {
  if (isActive()) {
    stop();
  }
}


/**
 * @brief Callback for new toplevel-window handle from WM.
 * 
 * @param toplevel (struct ::zwlr_foreign_toplevel_handle_v1*) The handle
 *                                                             mentioned above.
 * @details Creates a @c WlrToplevelHandle and connects the @c Actived/Closed
 *          signals to target slots.
 * @note Overridding the virtual function from 
 *       @c zwlr_foreign_toplevel_manager_v1.
 * @return void
 */

void WlrToplevelMgr::zwlr_foreign_toplevel_manager_v1_toplevel(struct
    ::zwlr_foreign_toplevel_handle_v1* toplevel) {
  auto* handle = new WlrToplevelHandle(toplevel, this);

  connect(handle, &WlrToplevelHandle::Activated,
    this, &WlrToplevelMgr::OnHandleActivated);
  connect(handle, &WlrToplevelHandle::Closed,
    this, &WlrToplevelMgr::OnHandleClosed);
}


/**
 * @brief Handle the (window) activated event reported by some handles.
 * 
 * @param app_id (const QString &) The application ID.
 * @param title (const QString &) The window title.
 * @details Forward directly as @c TopLevelActivated(app_id, @c title).
 * @return void
 */

void WlrToplevelMgr::OnHandleActivated(const QString& app_id,
    const QString& title) {
  emit TopLevelActivated(app_id, title);
}


/**
 * @brief Handle the handle closed event.
 * 
 * @param handle (WlrToplevelHandle*) The handle object that has been closed.
 * @details Call @c deleteLater() to delete the handle later.
 * @return void
 */

void WlrToplevelMgr::OnHandleClosed(WlrToplevelHandle* handle) {
  handle->deleteLater();
}

/* ------------------------------------------------------------------------- */
/* Class 'CurrentWindowWlrootsImpl' */
/* ------------------------------------------------------------------------- */


/**
 * @brief Constructor of @c CurrentWindowWlrootsImpl.
 * 
 * @param parent (QObject*) The parent QObject.
 * @details Connect the @c TopLevelActivated signal of the manager.
 * @return void
 */

CurrentWindowWlrootsImpl::CurrentWindowWlrootsImpl(QObject* parent)
    : CurrentWindowProvider(parent), manager_(new WlrToplevelMgr()) {
  connect(manager_, &WlrToplevelMgr::TopLevelActivated,
    this, &CurrentWindowWlrootsImpl::OnTopLevelActivated);

  LOG(INFO) << absl::StrCat("Constructor of CurrentWindowWlrootsImpl has ",
    "been invoked!!\n ", "Extension active = ",
    manager_->isActive() ? "TRUE" : "FALSE", ".");
}


/**
 * @brief Handle the active window change from WM.
 * 
 * @param app_id (const QString &) The application ID.
 * @param title (const QString &) The window title.
 * @details Try to use @c KService to map @c app_id to a friendly app name. If
 *          failed, then use @c app_id as the display name. If even that fails,
 *          switch to @c title as the last resort. After information is
 *          gathered, emit the signal @c CurrentWindowChanged().
 * @return void
 */

void CurrentWindowWlrootsImpl::OnTopLevelActivated(const QString &app_id,
    const QString &title) {
  LOG(INFO) << absl::StrCat("Active window has been changed!!");

  KService::Ptr service = KService::serviceByDesktopName(app_id);
  if (!service) {
    service = KService::serviceByDesktopName(app_id.toLower());
  }

  if (service) {
    window_info_.application_name = service->name();
  } else {
    // No .desktop entry found
    window_info_.application_name = app_id.isEmpty() ? title : app_id;
  }

  window_info_.package_name = app_id;
  emit CurrentWindowChanged();
}


/**
 * @brief Gets the "friendly" application name of the current active window.
 * 
 * @details Returns the application name of the top-level window.
 * @see @c OnTopLevelActivated() for how that name is gathered.
 * @return (QString) The friendly application name.
 */

QString CurrentWindowWlrootsImpl::GetApplicationName() {
  return window_info_.application_name;
}


/**
 * @brief Gets the application package name of the current active window.
 * 
 * @details Returns the package name of the application for the top-level
 *          window.
 * @return (QString) The application package name.
 */

QString CurrentWindowWlrootsImpl::GetPackageName() {
  return window_info_.package_name;
}

}  // namespace backend
}  // namespace panel
