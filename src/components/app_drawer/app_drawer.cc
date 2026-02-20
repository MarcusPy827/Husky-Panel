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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QGridLayout>
#include <QSpacerItem>
#include <QScrollArea>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/components/app_drawer/app_drawer.h"
#include "src/font_loader/font_loader.h"
#include "src/info_server/user_info/user_info.h"
#include "src/utils/misc/misc.h"

#include "lib/3rdparty/layer-shell-qt/src/interfaces/window.h"

namespace panel {
namespace frontend {

AppDrawer::AppDrawer(QWidget *parent) : QWidget(parent) {
  setWindowTitle(tr("App Drawer"));
  setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
  setAttribute(Qt::WA_TranslucentBackground);

  winId();
  QWindow * native_handle = windowHandle();
  if (native_handle) {
    LayerShellQt::Window * layer_shell = LayerShellQt::Window::get(
      native_handle);
    layer_shell->setAnchors(static_cast<LayerShellQt::Window::Anchor>(
      LayerShellQt::Window::Anchor::AnchorTop |
      LayerShellQt::Window::Anchor::AnchorLeft));
    layer_shell->setMargins({8, 8, 0, 0});
  } else {
    LOG(WARNING) << absl::StrCat("Failed to get native window handle. ",
      "Calendar will pop up as a normal window...");
  }

  QHBoxLayout * layout_gen = new QHBoxLayout();
  layout_gen->setContentsMargins(0, 0, 0, 0);
  layout_gen->setSpacing(0);
  setLayout(layout_gen);

  QWidget * base_layer = new QWidget();
  base_layer->setProperty("class", "calendar_frame");
  layout_gen->addWidget(base_layer);

  QGraphicsDropShadowEffect * drop_shadow = new QGraphicsDropShadowEffect(
    base_layer);
  drop_shadow->setBlurRadius(12);
  drop_shadow->setColor("#88888888");
  drop_shadow->setOffset(0, 0);
  base_layer->setGraphicsEffect(drop_shadow);

  QVBoxLayout * base_layout = new QVBoxLayout();
  base_layout->setContentsMargins(2, 2, 2, 2);
  base_layout->setSpacing(0);
  base_layer->setLayout(base_layout);

  QWidget * title_bar = new QWidget();
  title_bar->setProperty("class", "app_bar_container_app_drawer");
  base_layout->addWidget(title_bar);

  QHBoxLayout * title_bar_layout = new QHBoxLayout();
  title_bar_layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  title_bar_layout->setContentsMargins(16, 0, 16, 0);
  title_bar->setLayout(title_bar_layout);

  if (menu_btn_ == nullptr) {
    menu_btn_ = new QToolButton();
    menu_btn_->setProperty("class", "app_bar_icon_button");
  }
  menu_btn_->setText("menu");
  menu_btn_->setFont(loader::FontLoader::GetRoundedMaterialSymbolFont());
  title_bar_layout->addWidget(menu_btn_);

  QGridLayout * search_bar_layout_internal = new QGridLayout();
  search_bar_layout_internal->setContentsMargins(0, 0, 0, 0);
  search_bar_layout_internal->setSpacing(0);
  title_bar_layout->addLayout(search_bar_layout_internal);

  if (search_bar_internal_ == nullptr) {
    search_bar_internal_ = new QLineEdit();
    search_bar_internal_->setProperty("class", "app_bar_search_bar");
  }
  search_bar_internal_->setPlaceholderText(tr("Search..."));
  search_bar_internal_->setVisible(true);
  search_bar_layout_internal->addWidget(search_bar_internal_, 0, 0);

  if (search_bar_icon_btn_internal_ == nullptr) {
    search_bar_icon_btn_internal_ = new QToolButton();
    search_bar_icon_btn_internal_->setProperty("class", "app_bar_icon_button");
  }
  search_bar_icon_btn_internal_->setText("search");
  search_bar_icon_btn_internal_->setFont(
    loader::FontLoader::GetRoundedMaterialSymbolFont());

  QHBoxLayout * search_bar_icon_btn_layout_internal = new QHBoxLayout();
  search_bar_icon_btn_layout_internal->setContentsMargins(0, 0, 0, 0);
  search_bar_icon_btn_layout_internal->setSpacing(0);
  search_bar_layout_internal->addLayout(search_bar_icon_btn_layout_internal, 0,
    0, Qt::AlignRight | Qt::AlignVCenter);
  search_bar_icon_btn_internal_->setVisible(true);
  search_bar_icon_btn_layout_internal->addWidget(
        search_bar_icon_btn_internal_);

  QSpacerItem * search_bar_icon_btn_padding = new QSpacerItem(4, 1,
    QSizePolicy::Fixed, QSizePolicy::Fixed);
  search_bar_icon_btn_layout_internal->addSpacerItem(
    search_bar_icon_btn_padding);

  if (avatar_btn_ == nullptr) {
    avatar_btn_ = new QToolButton();
    avatar_btn_->setProperty("class", "app_bar_icon_button");
  }
  avatar_btn_->setIcon(QIcon(utils::Misc::CropCirclePixmap(
    utils::Misc::ScalePixmap(QPixmap(backend::UserInfo::GetUserAvatarPath()),
    48, 48))));
  avatar_btn_->setIconSize(QSize(48, 48));
  title_bar_layout->addWidget(avatar_btn_);

  QHBoxLayout * drawer_menu_content_view = new QHBoxLayout();
  drawer_menu_content_view->setContentsMargins(0, 0, 0, 0);
  drawer_menu_content_view->setSpacing(0);
  base_layout->addLayout(drawer_menu_content_view);

  QScrollArea * side_pane_view_container = new QScrollArea();
  side_pane_view_container->setWidgetResizable(true);
  drawer_menu_content_view->addWidget(side_pane_view_container);

  if (side_pane_ == nullptr) {
    side_pane_ = new QWidget();
    side_pane_->setProperty("class", "navigation_drawer_compact");
  }
  side_pane_view_container->setMaximumHeight(400);
  side_pane_view_container->setWidget(side_pane_);

  QVBoxLayout * side_pane_layout = new QVBoxLayout();
  side_pane_layout->setContentsMargins(16, 16, 16, 16);
  side_pane_layout->setSpacing(6);
  side_pane_->setLayout(side_pane_layout);

  if (all_apps_btn_ == nullptr) {
    all_apps_btn_ = new AppDrawerSidePaneItem("apps", tr("All applications"),
      "app_drawer_side_pane", "all_apps", true);
  }
  side_pane_layout->addWidget(all_apps_btn_);

  if (audio_video_apps_btn_ == nullptr) {
    audio_video_apps_btn_ = new AppDrawerSidePaneItem("movie", tr("Multimedia"),
      "app_drawer_side_pane", "multimedia", false);
  }
  side_pane_layout->addWidget(audio_video_apps_btn_);

  if (development_apps_btn_ == nullptr) {
    development_apps_btn_ = new AppDrawerSidePaneItem("construction",
      tr("Development"), "app_drawer_side_pane", "development", false);
  }
  side_pane_layout->addWidget(development_apps_btn_);

  if (education_apps_btn_ == nullptr) {
    education_apps_btn_ = new AppDrawerSidePaneItem("school",
      tr("Education"), "app_drawer_side_pane", "education", false);
  }
  side_pane_layout->addWidget(education_apps_btn_);

  if (game_apps_btn_ == nullptr) {
    game_apps_btn_ = new AppDrawerSidePaneItem("stadia_controller",
      tr("Games"), "app_drawer_side_pane", "game", false);
  }
  side_pane_layout->addWidget(game_apps_btn_);

  if (graphics_apps_btn_ == nullptr) {
    graphics_apps_btn_ = new AppDrawerSidePaneItem("photo",
      tr("Graphics"), "app_drawer_side_pane", "graphics", false);
  }
  side_pane_layout->addWidget(graphics_apps_btn_);

  if (network_apps_btn_== nullptr) {
    network_apps_btn_ = new AppDrawerSidePaneItem("lan",
      tr("Network"), "app_drawer_side_pane", "network", false);
  }
  side_pane_layout->addWidget(network_apps_btn_);

  if (office_apps_btn_ == nullptr) {
    office_apps_btn_ = new AppDrawerSidePaneItem("domain",
      tr("Office"), "app_drawer_side_pane", "office", false);
  }
  side_pane_layout->addWidget(office_apps_btn_);

  if (settings_apps_btn_ == nullptr) {
    settings_apps_btn_ = new AppDrawerSidePaneItem("settings",
      tr("Settings"), "app_drawer_side_pane", "settings", false);
  }
  side_pane_layout->addWidget(settings_apps_btn_);

  if (system_apps_btn_ == nullptr) {
    system_apps_btn_ = new AppDrawerSidePaneItem("computer",
      tr("System"), "app_drawer_side_pane", "system", false);
  }
  side_pane_layout->addWidget(system_apps_btn_);

  if (utility_apps_btn_ == nullptr) {
    utility_apps_btn_ = new AppDrawerSidePaneItem("service_toolbox",
      tr("Utilities"), "app_drawer_side_pane", "utility", false);
  }
  side_pane_layout->addWidget(utility_apps_btn_);

  QWidget * tool_bar = new QWidget();
  tool_bar->setProperty("class", "tool_bar_app_drawer");
  base_layout->addWidget(tool_bar);

  QHBoxLayout * tool_bar_layout = new QHBoxLayout();
  tool_bar_layout->setContentsMargins(16, 0, 16, 0);
  tool_bar_layout->setSpacing(32);
  tool_bar_layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  tool_bar->setLayout(tool_bar_layout);

  if (sleep_btn_ == nullptr) {
    sleep_btn_ = new QToolButton();
    sleep_btn_->setProperty("class", "app_bar_icon_button");
  }
  sleep_btn_->setText("bedtime");
  sleep_btn_->setToolTip(tr("Sleep"));
  sleep_btn_->setVisible(true);
  sleep_btn_->setFont(loader::FontLoader::GetRoundedMaterialSymbolFont());
  tool_bar_layout->addWidget(sleep_btn_);

  if (hibernate_btn_ == nullptr) {
    hibernate_btn_ = new QToolButton();
    hibernate_btn_->setProperty("class", "app_bar_icon_button");
  }
  hibernate_btn_->setText("ac_unit");
  hibernate_btn_->setToolTip(tr("Hibernate"));
  hibernate_btn_->setVisible(true);
  hibernate_btn_->setFont(loader::FontLoader::GetRoundedMaterialSymbolFont());
  tool_bar_layout->addWidget(hibernate_btn_);

  if (restart_btn_ == nullptr) {
    restart_btn_ = new QToolButton();
    restart_btn_->setProperty("class", "app_bar_icon_button");
  }
  restart_btn_->setText("restart_alt");
  restart_btn_->setToolTip(tr("Reboot"));
  restart_btn_->setVisible(true);
  restart_btn_->setFont(loader::FontLoader::GetRoundedMaterialSymbolFont());
  tool_bar_layout->addWidget(restart_btn_);

  if (shut_down_btn_ == nullptr) {
    shut_down_btn_ = new QToolButton();
    shut_down_btn_->setProperty("class", "app_bar_icon_button");
  }
  shut_down_btn_->setText("power_settings_new");
  shut_down_btn_->setToolTip(tr("Shut down"));
  shut_down_btn_->setVisible(true);
  shut_down_btn_->setFont(loader::FontLoader::GetRoundedMaterialSymbolFont());
  tool_bar_layout->addWidget(shut_down_btn_);

  QSpacerItem * tool_bar_spacer = new QSpacerItem(8, 8, QSizePolicy::Expanding,
    QSizePolicy::Expanding);
  // tool_bar_layout->addSpacerItem(tool_bar_spacer);

  if (session_btn_ == nullptr) {
    session_btn_ = new QToolButton();
    session_btn_->setProperty("class", "fab_48px");
  }
  session_btn_->setText("account_circle");
  session_btn_->setToolTip(tr("Session"));
  session_btn_->setVisible(true);
  session_btn_->setFont(loader::FontLoader::GetRoundedMaterialSymbolFont());
  tool_bar_layout->addWidget(session_btn_);

  QString highlight_color = utils::ColorPaletteWrapper::
    GetSystemHighlightColor().name();
  if (theme_loader_ == nullptr) {
    theme_loader_ = new loader::ThemeLoader(base_layer, highlight_color, this);
  }
}

AppDrawer::~AppDrawer() {
  LOG(INFO) << absl::StrCat("AppDrawer is being deleted.");
}

}  // namespace frontend
}  // namespace panel
