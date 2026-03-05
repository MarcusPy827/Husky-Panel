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
#include "src/info_server/application_info/application_info.h"
#include "src/utils/misc/misc.h"

#include "lib/3rdparty/layer-shell-qt/src/interfaces/window.h"

namespace panel {
namespace frontend {

AppDrawer::AppDrawer(QWidget *parent) : QWidget(parent) {
  if (translator_ == nullptr) {
    translator_ = new loader::TranslationLoader(
      ":/translations/translations/app_drawer.locale",
    loader::LanguageType::EN_US);
  }

  setWindowTitle(Tr("AppDrawer"));
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
  QObject::connect(menu_btn_, &QToolButton::clicked, this,
    &AppDrawer::ToggleSidePane);
  title_bar_layout->addWidget(menu_btn_);

  QGridLayout * search_bar_layout_internal = new QGridLayout();
  search_bar_layout_internal->setContentsMargins(0, 0, 0, 0);
  search_bar_layout_internal->setSpacing(0);
  title_bar_layout->addLayout(search_bar_layout_internal);

  if (search_bar_internal_ == nullptr) {
    search_bar_internal_ = new QLineEdit();
    search_bar_internal_->setProperty("class", "app_bar_search_bar");
    QObject::connect(search_bar_internal_, &QLineEdit::textChanged, this,
      &AppDrawer::OnSearchBarTextChanged);
  }
  search_bar_internal_->setPlaceholderText(Tr("SearchHere"));
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

  QWidget * side_pane_view_base_layer = new QWidget();
  side_pane_view_base_layer->setProperty("class", "side_pane_view_base_layer");
  drawer_menu_content_view->addWidget(side_pane_view_base_layer);

  QHBoxLayout * side_pane_view_base_layer_layout = new QHBoxLayout();
  side_pane_view_base_layer_layout->setContentsMargins(0, 0, 0, 0);
  side_pane_view_base_layer_layout->setSpacing(0);
  side_pane_view_base_layer->setLayout(side_pane_view_base_layer_layout);

  if (side_pane_view_container == nullptr) {
    side_pane_view_container = new QScrollArea();
    side_pane_view_container->setWidgetResizable(true);
    side_pane_view_container->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    side_pane_view_container->setHorizontalScrollBarPolicy(
      Qt::ScrollBarAlwaysOff);
  }
  side_pane_view_base_layer_layout->addWidget(side_pane_view_container);

  if (side_pane_ == nullptr) {
    side_pane_ = new QWidget();
    side_pane_->setProperty("class", "navigation_drawer_compact");
  }
  side_pane_view_container->setMaximumHeight(400);
  side_pane_view_container->setWidget(side_pane_);

  QVBoxLayout * side_pane_layout = new QVBoxLayout();
  side_pane_layout->setContentsMargins(16, 0, 16, 16);
  side_pane_layout->setSpacing(6);
  side_pane_->setLayout(side_pane_layout);

  if (all_apps_btn_ == nullptr) {
    all_apps_btn_ = new AppDrawerSidePaneItem("apps", Tr("AllApps"),
      "app_drawer_side_pane", "all_apps", true);
    side_pane_items_.append(all_apps_btn_);
  }
  side_pane_layout->addWidget(all_apps_btn_);

  if (search_apps_btn_ == nullptr) {
    search_apps_btn_ = new AppDrawerSidePaneItem("search", Tr("SearchApps"),
      "app_drawer_side_pane", "search_apps", false);
    side_pane_items_.append(search_apps_btn_);
  }
  side_pane_layout->addWidget(search_apps_btn_);

  if (audio_video_apps_btn_ == nullptr) {
    audio_video_apps_btn_ = new AppDrawerSidePaneItem("movie", Tr("Multimedia"),
      "app_drawer_side_pane", "multimedia", false);
    side_pane_items_.append(audio_video_apps_btn_);
  }
  side_pane_layout->addWidget(audio_video_apps_btn_);

  if (development_apps_btn_ == nullptr) {
    development_apps_btn_ = new AppDrawerSidePaneItem("construction",
      Tr("Development"), "app_drawer_side_pane", "development", false);
    side_pane_items_.append(development_apps_btn_);
  }
  side_pane_layout->addWidget(development_apps_btn_);

  if (education_apps_btn_ == nullptr) {
    education_apps_btn_ = new AppDrawerSidePaneItem("school",
      Tr("Education"), "app_drawer_side_pane", "education", false);
    side_pane_items_.append(education_apps_btn_);
  }
  side_pane_layout->addWidget(education_apps_btn_);

  if (game_apps_btn_ == nullptr) {
    game_apps_btn_ = new AppDrawerSidePaneItem("stadia_controller",
      Tr("Games"), "app_drawer_side_pane", "game", false);
    side_pane_items_.append(game_apps_btn_);
  }
  side_pane_layout->addWidget(game_apps_btn_);

  if (graphics_apps_btn_ == nullptr) {
    graphics_apps_btn_ = new AppDrawerSidePaneItem("photo",
      Tr("Graphics"), "app_drawer_side_pane", "graphics", false);
    side_pane_items_.append(graphics_apps_btn_);
  }
  side_pane_layout->addWidget(graphics_apps_btn_);

  if (network_apps_btn_== nullptr) {
    network_apps_btn_ = new AppDrawerSidePaneItem("lan",
      Tr("Network"), "app_drawer_side_pane", "network", false);
    side_pane_items_.append(network_apps_btn_);
  }
  side_pane_layout->addWidget(network_apps_btn_);

  if (office_apps_btn_ == nullptr) {
    office_apps_btn_ = new AppDrawerSidePaneItem("domain",
      Tr("Office"), "app_drawer_side_pane", "office", false);
    side_pane_items_.append(office_apps_btn_);
  }
  side_pane_layout->addWidget(office_apps_btn_);

  if (settings_apps_btn_ == nullptr) {
    settings_apps_btn_ = new AppDrawerSidePaneItem("settings",
      Tr("Settings"), "app_drawer_side_pane", "settings", false);
    side_pane_items_.append(settings_apps_btn_);
  }
  side_pane_layout->addWidget(settings_apps_btn_);

  if (system_apps_btn_ == nullptr) {
    system_apps_btn_ = new AppDrawerSidePaneItem("computer",
      Tr("System"), "app_drawer_side_pane", "system", false);
    side_pane_items_.append(system_apps_btn_);
  }
  side_pane_layout->addWidget(system_apps_btn_);

  if (utility_apps_btn_ == nullptr) {
    utility_apps_btn_ = new AppDrawerSidePaneItem("service_toolbox",
      Tr("Utilities"), "app_drawer_side_pane", "utility", false);
    side_pane_items_.append(utility_apps_btn_);
  }
  side_pane_layout->addWidget(utility_apps_btn_);

  for (auto item : side_pane_items_) {
    if (item != nullptr) {
      QObject::connect(item, &AppDrawerSidePaneItem::GroupSelected, this,
          [this](const QString& group_identifier, const QString& id) {
        for (auto* target : side_pane_items_) {
          if (target != nullptr) {
            target->OnGroupUpdated(group_identifier, id);
            UpdatePaneContent(id);
          }
        }
      });
    }
  }

  QWidget * drawer_base_layer = new QWidget();
  drawer_base_layer->setProperty("class", "side_pane_view_base_layer");
  drawer_menu_content_view->addWidget(drawer_base_layer);

  QHBoxLayout * drawer_base_layer_layout = new QHBoxLayout();
  drawer_base_layer_layout->setContentsMargins(0, 0, 0, 0);
  drawer_base_layer_layout->setSpacing(0);
  drawer_base_layer->setLayout(drawer_base_layer_layout);

  if (drawer_stack_ == nullptr) {
    drawer_stack_ = new QStackedWidget();
    drawer_stack_->setProperty("class", "side_pane_view_base_layer");
  }
  drawer_base_layer_layout->addWidget(drawer_stack_);

  QScrollArea * drawer_container = new QScrollArea();
  drawer_container->setWidgetResizable(true);
  drawer_container->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  drawer_container->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  drawer_stack_->insertWidget(0, drawer_container);

  QWidget * actual_drawer = new QWidget();
  actual_drawer->setProperty("class", "navigation_drawer_compact");
  drawer_container->setMaximumHeight(400);
  drawer_container->setWidget(actual_drawer);

  if (drawer_layout_ == nullptr) {
    drawer_layout_ = new QGridLayout();
    drawer_layout_->setContentsMargins(16, 16, 16, 16);
    drawer_layout_->setSpacing(8);
  }
  actual_drawer->setLayout(drawer_layout_);

  // Search drawer (index 1)
  if (search_drawer_ == nullptr) {
    search_drawer_ = new QScrollArea();
    search_drawer_->setWidgetResizable(true);
    search_drawer_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    search_drawer_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
  drawer_stack_->insertWidget(1, search_drawer_);

  if (actual_search_drawer_ == nullptr) {
    actual_search_drawer_ = new QWidget();
    actual_search_drawer_->setProperty("class", "navigation_drawer_compact");
    actual_search_drawer_->setSizePolicy(QSizePolicy::Expanding,
      QSizePolicy::Expanding);
  }
  search_drawer_->setMaximumHeight(400);
  search_drawer_->setWidget(actual_search_drawer_);

  if (search_layout_ == nullptr) {
    search_layout_ = new QGridLayout();
    search_layout_->setContentsMargins(16, 16, 16, 16);
    search_layout_->setSpacing(8);
    search_layout_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  }
  actual_search_drawer_->setLayout(search_layout_);

  // Audio/Video drawer (index 2)
  if (audio_video_drawer_ == nullptr) {
    audio_video_drawer_ = new QScrollArea();
    audio_video_drawer_->setWidgetResizable(true);
    audio_video_drawer_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    audio_video_drawer_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
  drawer_stack_->insertWidget(2, audio_video_drawer_);

  if (actual_audio_video_drawer_ == nullptr) {
    actual_audio_video_drawer_ = new QWidget();
    actual_audio_video_drawer_->setProperty("class",
      "navigation_drawer_compact");
    actual_audio_video_drawer_->setSizePolicy(QSizePolicy::Expanding,
      QSizePolicy::Expanding);
  }
  audio_video_drawer_->setMaximumHeight(400);
  audio_video_drawer_->setWidget(actual_audio_video_drawer_);

  if (audio_video_layout_ == nullptr) {
    audio_video_layout_ = new QGridLayout();
    audio_video_layout_->setContentsMargins(16, 16, 16, 16);
    audio_video_layout_->setSpacing(8);
    audio_video_layout_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  }
  actual_audio_video_drawer_->setLayout(audio_video_layout_);

  // Development drawer (index 3)
  if (development_drawer_ == nullptr) {
    development_drawer_ = new QScrollArea();
    development_drawer_->setWidgetResizable(true);
    development_drawer_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    development_drawer_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
  drawer_stack_->insertWidget(3, development_drawer_);

  if (actual_development_drawer_ == nullptr) {
    actual_development_drawer_ = new QWidget();
    actual_development_drawer_->setProperty("class",
      "navigation_drawer_compact");
    actual_development_drawer_->setSizePolicy(QSizePolicy::Expanding,
      QSizePolicy::Expanding);
  }
  development_drawer_->setMaximumHeight(400);
  development_drawer_->setWidget(actual_development_drawer_);

  if (development_layout_ == nullptr) {
    development_layout_ = new QGridLayout();
    development_layout_->setContentsMargins(16, 16, 16, 16);
    development_layout_->setSpacing(8);
    development_layout_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  }
  actual_development_drawer_->setLayout(development_layout_);

  // Education drawer (index 4)
  if (education_drawer_ == nullptr) {
    education_drawer_ = new QScrollArea();
    education_drawer_->setWidgetResizable(true);
    education_drawer_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    education_drawer_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
  drawer_stack_->insertWidget(4, education_drawer_);

  if (actual_education_drawer_ == nullptr) {
    actual_education_drawer_ = new QWidget();
    actual_education_drawer_->setProperty("class", "navigation_drawer_compact");
    actual_education_drawer_->setSizePolicy(QSizePolicy::Expanding,
      QSizePolicy::Expanding);
  }
  education_drawer_->setMaximumHeight(400);
  education_drawer_->setWidget(actual_education_drawer_);

  if (education_layout_ == nullptr) {
    education_layout_ = new QGridLayout();
    education_layout_->setContentsMargins(16, 16, 16, 16);
    education_layout_->setSpacing(8);
    education_layout_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  }
  actual_education_drawer_->setLayout(education_layout_);

  // Game drawer (index 5)
  if (game_drawer_ == nullptr) {
    game_drawer_ = new QScrollArea();
    game_drawer_->setWidgetResizable(true);
    game_drawer_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    game_drawer_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
  drawer_stack_->insertWidget(5, game_drawer_);

  if (actual_game_drawer_ == nullptr) {
    actual_game_drawer_ = new QWidget();
    actual_game_drawer_->setProperty("class", "navigation_drawer_compact");
    actual_game_drawer_->setSizePolicy(QSizePolicy::Expanding,
      QSizePolicy::Expanding);
  }
  game_drawer_->setMaximumHeight(400);
  game_drawer_->setWidget(actual_game_drawer_);

  if (game_layout_ == nullptr) {
    game_layout_ = new QGridLayout();
    game_layout_->setContentsMargins(16, 16, 16, 16);
    game_layout_->setSpacing(8);
    game_layout_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  }
  actual_game_drawer_->setLayout(game_layout_);

  // Graphics drawer (index 6)
  if (graphics_drawer_ == nullptr) {
    graphics_drawer_ = new QScrollArea();
    graphics_drawer_->setWidgetResizable(true);
    graphics_drawer_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    graphics_drawer_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
  drawer_stack_->insertWidget(6, graphics_drawer_);

  if (actual_graphics_drawer_ == nullptr) {
    actual_graphics_drawer_ = new QWidget();
    actual_graphics_drawer_->setProperty("class", "navigation_drawer_compact");
    actual_graphics_drawer_->setSizePolicy(QSizePolicy::Expanding,
      QSizePolicy::Expanding);
  }
  graphics_drawer_->setMaximumHeight(400);
  graphics_drawer_->setWidget(actual_graphics_drawer_);

  if (graphics_layout_ == nullptr) {
    graphics_layout_ = new QGridLayout();
    graphics_layout_->setContentsMargins(16, 16, 16, 16);
    graphics_layout_->setSpacing(8);
    graphics_layout_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  }
  actual_graphics_drawer_->setLayout(graphics_layout_);

  // Network drawer (index 7)
  if (network_drawer_ == nullptr) {
    network_drawer_ = new QScrollArea();
    network_drawer_->setWidgetResizable(true);
    network_drawer_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    network_drawer_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
  drawer_stack_->insertWidget(7, network_drawer_);

  if (actual_network_drawer_ == nullptr) {
    actual_network_drawer_ = new QWidget();
    actual_network_drawer_->setProperty("class", "navigation_drawer_compact");
    actual_network_drawer_->setSizePolicy(QSizePolicy::Expanding,
      QSizePolicy::Expanding);
  }
  network_drawer_->setMaximumHeight(400);
  network_drawer_->setWidget(actual_network_drawer_);

  if (network_layout_ == nullptr) {
    network_layout_ = new QGridLayout();
    network_layout_->setContentsMargins(16, 16, 16, 16);
    network_layout_->setSpacing(8);
    network_layout_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  }
  actual_network_drawer_->setLayout(network_layout_);

  // Office drawer (index 8)
  if (office_drawer_ == nullptr) {
    office_drawer_ = new QScrollArea();
    office_drawer_->setWidgetResizable(true);
    office_drawer_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    office_drawer_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
  drawer_stack_->insertWidget(8, office_drawer_);

  if (actual_office_drawer_ == nullptr) {
    actual_office_drawer_ = new QWidget();
    actual_office_drawer_->setProperty("class", "navigation_drawer_compact");
    actual_office_drawer_->setSizePolicy(QSizePolicy::Expanding,
      QSizePolicy::Expanding);
  }
  office_drawer_->setMaximumHeight(400);
  office_drawer_->setWidget(actual_office_drawer_);

  if (office_layout_ == nullptr) {
    office_layout_ = new QGridLayout();
    office_layout_->setContentsMargins(16, 16, 16, 16);
    office_layout_->setSpacing(8);
    office_layout_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  }
  actual_office_drawer_->setLayout(office_layout_);

  // Settings drawer (index 9)
  if (settings_drawer_ == nullptr) {
    settings_drawer_ = new QScrollArea();
    settings_drawer_->setWidgetResizable(true);
    settings_drawer_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    settings_drawer_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
  drawer_stack_->insertWidget(9, settings_drawer_);

  if (actual_settings_drawer_ == nullptr) {
    actual_settings_drawer_ = new QWidget();
    actual_settings_drawer_->setProperty("class", "navigation_drawer_compact");
    actual_settings_drawer_->setSizePolicy(QSizePolicy::Expanding,
      QSizePolicy::Expanding);
  }
  settings_drawer_->setMaximumHeight(400);
  settings_drawer_->setWidget(actual_settings_drawer_);

  if (settings_layout_ == nullptr) {
    settings_layout_ = new QGridLayout();
    settings_layout_->setContentsMargins(16, 16, 16, 16);
    settings_layout_->setSpacing(8);
    settings_layout_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  }
  actual_settings_drawer_->setLayout(settings_layout_);

  // System drawer (index 10)
  if (system_drawer_ == nullptr) {
    system_drawer_ = new QScrollArea();
    system_drawer_->setWidgetResizable(true);
    system_drawer_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    system_drawer_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
  drawer_stack_->insertWidget(10, system_drawer_);

  if (actual_system_drawer_ == nullptr) {
    actual_system_drawer_ = new QWidget();
    actual_system_drawer_->setProperty("class", "navigation_drawer_compact");
    actual_system_drawer_->setSizePolicy(QSizePolicy::Expanding,
      QSizePolicy::Expanding);
  }
  system_drawer_->setMaximumHeight(400);
  system_drawer_->setWidget(actual_system_drawer_);

  if (system_layout_ == nullptr) {
    system_layout_ = new QGridLayout();
    system_layout_->setContentsMargins(16, 16, 16, 16);
    system_layout_->setSpacing(8);
    system_layout_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  }
  actual_system_drawer_->setLayout(system_layout_);

  // Utility drawer (index 11)
  if (utility_drawer_ == nullptr) {
    utility_drawer_ = new QScrollArea();
    utility_drawer_->setWidgetResizable(true);
    utility_drawer_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    utility_drawer_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
  drawer_stack_->insertWidget(11, utility_drawer_);

  if (actual_utility_drawer_ == nullptr) {
    actual_utility_drawer_ = new QWidget();
    actual_utility_drawer_->setProperty("class", "navigation_drawer_compact");
    actual_utility_drawer_->setSizePolicy(QSizePolicy::Expanding,
      QSizePolicy::Expanding);
  }
  utility_drawer_->setMaximumHeight(400);
  utility_drawer_->setWidget(actual_utility_drawer_);

  if (utility_layout_ == nullptr) {
    utility_layout_ = new QGridLayout();
    utility_layout_->setContentsMargins(16, 16, 16, 16);
    utility_layout_->setSpacing(8);
    utility_layout_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  }
  actual_utility_drawer_->setLayout(utility_layout_);

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
  tool_bar_layout->addSpacerItem(tool_bar_spacer);

  if (session_btn_ == nullptr) {
    session_btn_ = new QToolButton();
    session_btn_->setProperty("class", "fab_48px");
  }
  session_btn_->setText("account_circle");
  session_btn_->setToolTip(tr("Session"));
  session_btn_->setVisible(true);
  session_btn_->setFont(loader::FontLoader::GetRoundedMaterialSymbolFont());
  tool_bar_layout->addWidget(session_btn_);

  if (UpdateAppDrawerItems() == 0) {
    QString highlight_color = utils::ColorPaletteWrapper::
    GetSystemHighlightColor().name();
    if (theme_loader_ == nullptr) {
      theme_loader_ = new loader::ThemeLoader(base_layer, highlight_color,
        this);
    }
  }
}

AppDrawer::~AppDrawer() {
  LOG(INFO) << absl::StrCat("AppDrawer is being deleted.");
}

QString AppDrawer::Tr(const QString& msg) {
  if (translator_ == nullptr) {
    return msg;
  } else {
    return translator_->GetTranslation(msg);
  }
}

int AppDrawer::UpdateAppDrawerItems() {
  LOG(INFO) << absl::StrCat("Cleaning old buttons...");

  // Clean all layouts
  if (drawer_layout_ != nullptr) {
    for (auto old : drawer_layout_->findChildren<QWidget*> ({},
        Qt::FindDirectChildrenOnly)) {
      delete old;
    }
  }

  if (search_layout_ != nullptr) {
    for (auto old : search_layout_->findChildren<QWidget*> ({},
        Qt::FindDirectChildrenOnly)) {
      delete old;
    }
  }

  if (audio_video_layout_ != nullptr) {
    for (auto old : audio_video_layout_->findChildren<QWidget*> ({},
        Qt::FindDirectChildrenOnly)) {
      delete old;
    }
  }

  if (development_layout_ != nullptr) {
    for (auto old : development_layout_->findChildren<QWidget*> ({},
        Qt::FindDirectChildrenOnly)) {
      delete old;
    }
  }

  if (education_layout_ != nullptr) {
    for (auto old : education_layout_->findChildren<QWidget*> ({},
        Qt::FindDirectChildrenOnly)) {
      delete old;
    }
  }

  if (game_layout_ != nullptr) {
    for (auto old : game_layout_->findChildren<QWidget*> ({},
        Qt::FindDirectChildrenOnly)) {
      delete old;
    }
  }

  if (graphics_layout_ != nullptr) {
    for (auto old : graphics_layout_->findChildren<QWidget*> ({},
        Qt::FindDirectChildrenOnly)) {
      delete old;
    }
  }

  if (network_layout_ != nullptr) {
    for (auto old : network_layout_->findChildren<QWidget*> ({},
        Qt::FindDirectChildrenOnly)) {
      delete old;
    }
  }

  if (office_layout_ != nullptr) {
    for (auto old : office_layout_->findChildren<QWidget*> ({},
        Qt::FindDirectChildrenOnly)) {
      delete old;
    }
  }

  if (settings_layout_ != nullptr) {
    for (auto old : settings_layout_->findChildren<QWidget*> ({},
        Qt::FindDirectChildrenOnly)) {
      delete old;
    }
  }

  if (system_layout_ != nullptr) {
    for (auto old : system_layout_->findChildren<QWidget*> ({},
        Qt::FindDirectChildrenOnly)) {
      delete old;
    }
  }

  if (utility_layout_ != nullptr) {
    for (auto old : utility_layout_->findChildren<QWidget*> ({},
        Qt::FindDirectChildrenOnly)) {
      delete old;
    }
  }

  LOG(INFO) << absl::StrCat("Pulling installed application info...");
  QList<AppInfo> app_infos_ = backend::ApplicationInfo::GetAllAppications();
  int cur_col = 0;
  int cur_row = 0;

  // All apps drawer
  if (drawer_layout_ != nullptr) {
    cur_col = 0;
    cur_row = 0;
    for (const auto& app : app_infos_) {
      AppDrawerItem * item_gen = new AppDrawerItem(app);
      drawer_layout_->addWidget(item_gen, cur_row, cur_col);
      cur_col += 1;
      if (cur_col > 3) {
        cur_col = 0;
        cur_row += 1;
      }
    }
  }

  // Search drawer (uses all apps)
  if (search_layout_ != nullptr) {
    cur_col = 0;
    cur_row = 0;
    for (const auto& app : app_infos_) {
      AppDrawerItem * item_gen = new AppDrawerItem(app);
      search_layout_->addWidget(item_gen, cur_row, cur_col);
      cur_col += 1;
      if (cur_col > 3) {
        cur_col = 0;
        cur_row += 1;
      }
    }
  }

  // AudioVideo drawer
  if (audio_video_layout_ != nullptr) {
    cur_col = 0;
    cur_row = 0;
    QList<AppInfo> audio_video_app_infos =
      backend::ApplicationInfo::GetApplicationsByCategory("AudioVideo");
    for (const auto& app : audio_video_app_infos) {
      AppDrawerItem * item_gen = new AppDrawerItem(app);
      audio_video_layout_->addWidget(item_gen, cur_row, cur_col);
      cur_col += 1;
      if (cur_col > 3) {
        cur_col = 0;
        cur_row += 1;
      }
    }
  }

  // Development drawer
  if (development_layout_ != nullptr) {
    cur_col = 0;
    cur_row = 0;
    QList<AppInfo> development_app_infos =
      backend::ApplicationInfo::GetApplicationsByCategory("Development");
    for (const auto& app : development_app_infos) {
      AppDrawerItem * item_gen = new AppDrawerItem(app);
      development_layout_->addWidget(item_gen, cur_row, cur_col);
      cur_col += 1;
      if (cur_col > 3) {
        cur_col = 0;
        cur_row += 1;
      }
    }
  }

  // Education drawer
  if (education_layout_ != nullptr) {
    cur_col = 0;
    cur_row = 0;
    QList<AppInfo> education_app_infos =
      backend::ApplicationInfo::GetApplicationsByCategory("Education");
    for (const auto& app : education_app_infos) {
      AppDrawerItem * item_gen = new AppDrawerItem(app);
      education_layout_->addWidget(item_gen, cur_row, cur_col);
      cur_col += 1;
      if (cur_col > 3) {
        cur_col = 0;
        cur_row += 1;
      }
    }
  }

  // Game drawer
  if (game_layout_ != nullptr) {
    cur_col = 0;
    cur_row = 0;
    QList<AppInfo> game_app_infos =
      backend::ApplicationInfo::GetApplicationsByCategory("Game");
    for (const auto& app : game_app_infos) {
      AppDrawerItem * item_gen = new AppDrawerItem(app);
      game_layout_->addWidget(item_gen, cur_row, cur_col);
      cur_col += 1;
      if (cur_col > 3) {
        cur_col = 0;
        cur_row += 1;
      }
    }
  }

  // Graphics drawer
  if (graphics_layout_ != nullptr) {
    cur_col = 0;
    cur_row = 0;
    QList<AppInfo> graphics_app_infos =
      backend::ApplicationInfo::GetApplicationsByCategory("Graphics");
    for (const auto& app : graphics_app_infos) {
      AppDrawerItem * item_gen = new AppDrawerItem(app);
      graphics_layout_->addWidget(item_gen, cur_row, cur_col);
      cur_col += 1;
      if (cur_col > 3) {
        cur_col = 0;
        cur_row += 1;
      }
    }
  }

  // Network drawer
  if (network_layout_ != nullptr) {
    cur_col = 0;
    cur_row = 0;
    QList<AppInfo> network_app_infos =
      backend::ApplicationInfo::GetApplicationsByCategory("Network");
    for (const auto& app : network_app_infos) {
      AppDrawerItem * item_gen = new AppDrawerItem(app);
      network_layout_->addWidget(item_gen, cur_row, cur_col);
      cur_col += 1;
      if (cur_col > 3) {
        cur_col = 0;
        cur_row += 1;
      }
    }
  }

  // Office drawer
  if (office_layout_ != nullptr) {
    cur_col = 0;
    cur_row = 0;
    QList<AppInfo> office_app_infos =
      backend::ApplicationInfo::GetApplicationsByCategory("Office");
    for (const auto& app : office_app_infos) {
      AppDrawerItem * item_gen = new AppDrawerItem(app);
      office_layout_->addWidget(item_gen, cur_row, cur_col);
      cur_col += 1;
      if (cur_col > 3) {
        cur_col = 0;
        cur_row += 1;
      }
    }
  }

  // Settings drawer
  if (settings_layout_ != nullptr) {
    cur_col = 0;
    cur_row = 0;
    QList<AppInfo> settings_app_infos =
      backend::ApplicationInfo::GetApplicationsByCategory("Settings");
    for (const auto& app : settings_app_infos) {
      AppDrawerItem * item_gen = new AppDrawerItem(app);
      settings_layout_->addWidget(item_gen, cur_row, cur_col);
      cur_col += 1;
      if (cur_col > 3) {
        cur_col = 0;
        cur_row += 1;
      }
    }
  }

  // System drawer
  if (system_layout_ != nullptr) {
    cur_col = 0;
    cur_row = 0;
    QList<AppInfo> system_app_infos =
      backend::ApplicationInfo::GetApplicationsByCategory("System");
    for (const auto& app : system_app_infos) {
      AppDrawerItem * item_gen = new AppDrawerItem(app);
      system_layout_->addWidget(item_gen, cur_row, cur_col);
      cur_col += 1;
      if (cur_col > 3) {
        cur_col = 0;
        cur_row += 1;
      }
    }
  }

  // Utility drawer
  if (utility_layout_ != nullptr) {
    cur_col = 0;
    cur_row = 0;
    QList<AppInfo> utility_app_infos =
      backend::ApplicationInfo::GetApplicationsByCategory("Utility");
    for (const auto& app : utility_app_infos) {
      AppDrawerItem * item_gen = new AppDrawerItem(app);
      utility_layout_->addWidget(item_gen, cur_row, cur_col);
      cur_col += 1;
      if (cur_col > 3) {
        cur_col = 0;
        cur_row += 1;
      }
    }
  }

  return 0;
}

void AppDrawer::OnSearchBarTextChanged(const QString& text) {
  emit DrawerItemFilterUpdated(text);
}

void AppDrawer::UpdatePaneContent(const QString& id) {
  if (drawer_stack_ == nullptr) {
    LOG(ERROR) << absl::StrCat(
      "Drawer stack is null pointer, now aborting...");
    return;
  }

  if (id == "all_apps") {
    drawer_stack_->setCurrentIndex(0);
  } else if (id == "search_apps") {
    drawer_stack_->setCurrentIndex(1);
  } else if (id == "multimedia") {
    drawer_stack_->setCurrentIndex(2);
  } else if (id == "development") {
    drawer_stack_->setCurrentIndex(3);
  } else if (id == "education") {
    drawer_stack_->setCurrentIndex(4);
  } else if (id == "game") {
    drawer_stack_->setCurrentIndex(5);
  } else if (id == "graphics") {
    drawer_stack_->setCurrentIndex(6);
  } else if (id == "network") {
    drawer_stack_->setCurrentIndex(7);
  } else if (id == "office") {
    drawer_stack_->setCurrentIndex(8);
  } else if (id == "settings") {
    drawer_stack_->setCurrentIndex(9);
  } else if (id == "system") {
    drawer_stack_->setCurrentIndex(10);
  } else if (id == "utility") {
    drawer_stack_->setCurrentIndex(11);
  } else {
    LOG(INFO) << absl::StrCat("Unknown id '",
      id.toStdString(), "' defaulting to all_apps...");
    drawer_stack_->setCurrentIndex(0);
  }
}

void AppDrawer::ToggleSidePane() {
  if (side_pane_view_container == nullptr) {
    LOG(ERROR) << absl::StrCat("Side pane is null pointer, ",
      "now aborting...");
    return;
  }

  if (side_pane_view_container->isVisible()) {
    side_pane_view_container->hide();
  } else {
    side_pane_view_container->show();
  }
}

}  // namespace frontend
}  // namespace panel
