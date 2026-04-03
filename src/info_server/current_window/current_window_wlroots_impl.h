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

#ifndef SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_WLROOTS_IMPL_H_
#define SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_WLROOTS_IMPL_H_

#include <QObject>
#include <QString>
#include <QtWaylandClient/QWaylandClientExtension>

#include "qwayland-wlr-foreign-toplevel-management-unstable-v1.h"
#include "src/info_server/current_window/current_window_provider.h"

namespace panel {
namespace backend {

class WlrootsHelper : public QObject,
    public QtWayland::zwlr_foreign_toplevel_handle_v1 {
  Q_OBJECT

 public:
  explicit WlrootsHelper(struct ::zwlr_foreign_toplevel_handle_v1* handle,
    QObject* parent = nullptr);
  ~WlrootsHelper();

 protected:
  void zwlr_foreign_toplevel_handle_v1_title(const QString& title) override;
  void zwlr_foreign_toplevel_handle_v1_app_id(const QString& app_id) override;
  void zwlr_foreign_toplevel_handle_v1_state(wl_array* state) override;
  void zwlr_foreign_toplevel_handle_v1_done() override;
  void zwlr_foreign_toplevel_handle_v1_closed() override;

 private:
  QString pending_title_;
  QString pending_app_id_;
  bool pending_activated_ = false;

 signals:
  void Activated(const QString& app_id, const QString& title);
  void Closed(WlrootsHelper* self);
};

using WlrToplevelHandle = WlrootsHelper;

class WlrToplevelMgr
    : public QWaylandClientExtensionTemplate<WlrToplevelMgr>,
    public QtWayland::zwlr_foreign_toplevel_manager_v1 {
  Q_OBJECT

 public:
  explicit WlrToplevelMgr();
  ~WlrToplevelMgr();

 protected:
  void zwlr_foreign_toplevel_manager_v1_toplevel(
      struct ::zwlr_foreign_toplevel_handle_v1 *toplevel) override;

 private slots:
  void OnHandleActivated(const QString& app_id, const QString& title);
  void OnHandleClosed(WlrToplevelHandle* handle);

 signals:
  void TopLevelActivated(const QString& app_id, const QString& title);
};


class CurrentWindowWlrootsImpl : public CurrentWindowProvider {
  Q_OBJECT

 public:
  explicit CurrentWindowWlrootsImpl(QObject* parent = nullptr);
  ~CurrentWindowWlrootsImpl() override = default;

  QString GetApplicationName() override;
  QString GetPackageName() override;

 private:
  WlrToplevelMgr* manager_;
  CurrentWindowInfo window_info_;

 private slots:
  void OnTopLevelActivated(const QString& app_id, const QString& title);
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_CURRENT_WINDOW_CURRENT_WINDOW_WLROOTS_IMPL_H_
