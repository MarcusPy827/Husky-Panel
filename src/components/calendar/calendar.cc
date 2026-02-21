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

#include <QVBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QToolButton>
#include <QIcon>
#include <QGraphicsDropShadowEffect>
#include <QTableView>
#include <QHeaderView>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/components/calendar/calendar.h"
#include "src/font_loader/font_loader.h"

#include "lib/3rdparty/layer-shell-qt/src/interfaces/window.h"

namespace panel {
namespace frontend {

Calendar::Calendar(QWidget *parent) : QWidget(parent) {
  if (translator_ == nullptr) {
    translator_ = new loader::TranslationLoader(
      ":/translations/translations/calendar.locale",
      loader::LanguageType::EN_US);
  }

  if (translator_ == nullptr) {
    setWindowTitle("Calendar");
  } else {
    setWindowTitle(translator_->GetTranslation("Calendar"));
  }

  setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
  setAttribute(Qt::WA_TranslucentBackground);

  winId();
  QWindow * native_handle = windowHandle();
  if (native_handle) {
    LayerShellQt::Window * layer_shell = LayerShellQt::Window::get(
      native_handle);
    layer_shell->setAnchors(static_cast<LayerShellQt::Window::Anchor>(
      LayerShellQt::Window::Anchor::AnchorTop |
      LayerShellQt::Window::Anchor::AnchorRight));
    layer_shell->setMargins({0, 8, 8, 0});
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
  title_bar->setProperty("class", "calendar_title_bar");
  base_layout->addWidget(title_bar);

  QHBoxLayout * title_bar_layout = new QHBoxLayout();
  title_bar_layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  title_bar_layout->setContentsMargins(8, 0, 8, 0);
  title_bar->setLayout(title_bar_layout);

  QLabel * window_title = new QLabel();
  window_title->setProperty("class", "calendar_window_title");
  if (translator_ == nullptr) {
    window_title->setText("Calendar");
  } else {
    window_title->setText(translator_->GetTranslation("Calendar"));
  }
  title_bar_layout->addWidget(window_title);

  if (cal_ == nullptr) {
    cal_ = new QCalendarWidget();
    cal_->setProperty("class", "calendar_w");
  }

  QFont icon_font = loader::FontLoader::GetRoundedMaterialSymbolFont();
  icon_font.setStyleStrategy(QFont::PreferAntialias);
  auto prev_btn = cal_->findChild<QToolButton*>("qt_calendar_prevmonth");
  if (prev_btn) {
    prev_btn->setIcon(QIcon());
    prev_btn->setProperty("class", "calendar_handle_month_btn");
    prev_btn->setText(QChar(0xe5cb));
    prev_btn->setFont(icon_font);
  }

  auto next_btn = cal_->findChild<QToolButton*>("qt_calendar_nextmonth");
  if (next_btn) {
    next_btn->setIcon(QIcon());
    next_btn->setProperty("class", "calendar_handle_month_btn");
    next_btn->setText(QChar(0xe5cc));
    next_btn->setFont(icon_font);
  }

  auto month_btn = cal_->findChild<QToolButton*>("qt_calendar_monthbutton");
  if (month_btn) {
    month_btn->setProperty("class", "calendar_handle_time_btn");
  }

  auto year_btn = cal_->findChild<QToolButton*>("qt_calendar_yearbutton");
  if (year_btn) {
    year_btn->setProperty("class", "calendar_handle_time_btn");
  }

  auto cal_view = cal_->findChild<QTableView*>("qt_calendar_calendarview");
  if (cal_view) {
    QHeaderView * vert_header = cal_view->verticalHeader();
    if (vert_header) {
      vert_header->setAttribute(Qt::WA_StyledBackground, true);
      vert_header->setAutoFillBackground(false);
      vert_header->viewport()->setAutoFillBackground(false);
      vert_header->setProperty("class", "cal_header");
      vert_header->setStyleSheet("background: transparent");
    }
  }

  base_layout->addWidget(cal_);

  QString highlight_color = utils::ColorPaletteWrapper::
    GetSystemHighlightColor().name();
  if (theme_loader_ == nullptr) {
    theme_loader_ = new loader::ThemeLoader(base_layer, highlight_color, this);
  }
}

Calendar::~Calendar() {
  LOG(INFO) << absl::StrCat("ClockBtn is being deleted.");
}

}  // namespace frontend
}  // namespace panel
