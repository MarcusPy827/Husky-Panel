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
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QProcess>
#include <QRegularExpression>
#include <QMouseEvent>
#include <QFontMetrics>
#include <QSizePolicy>
#include <QMargins>

#include <string>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/components/app_drawer/app_drawer_search_item.h"

namespace panel {
namespace frontend {

AppDrawerSearchItem::AppDrawerSearchItem(const AppInfo& info, QWidget
    *parent) : QWidget(parent) {
  this->info_ = info;
  this->setProperty("class", "app_drawer_search_item");
  this->setCursor(Qt::PointingHandCursor);

  QHBoxLayout * layout_gen = new QHBoxLayout;
  layout_gen->setContentsMargins(0, 0, 0, 0);
  layout_gen->setSpacing(6);
  layout_gen->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  this->setLayout(layout_gen);

  if (btn_ == nullptr) {
    btn_ = new QToolButton();
    btn_->setProperty("class", "drawer_item_btn");
    btn_->setIconSize(QSize(48, 48));
    btn_->setMinimumSize(QSize(48, 48));
  }
  btn_->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  btn_->setIcon(QIcon::fromTheme(info_.icon_name));
  layout_gen->addWidget(btn_);

  QVBoxLayout * info_layout = new QVBoxLayout;
  info_layout->setContentsMargins(0, 0, 0, 0);
  info_layout->setSpacing(0);
  layout_gen->addLayout(info_layout);

  if (title_ == nullptr) {
    title_ = new QLabel();
    title_->setProperty("class", "app_drawer_item_label");
    title_->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  }
  title_->setText("<b>" + info_.name + "</b>");
  info_layout->addWidget(title_);

  if (!info_.comment.isEmpty()) {
    if (comment_ == nullptr) {
      comment_ = new QLabel();
      comment_->setProperty("class", "app_drawer_item_label");
      comment_->setAttribute(Qt::WA_TransparentForMouseEvents, true);
      comment_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
      comment_->setMinimumWidth(0);
      comment_->setWordWrap(false);
    }
    raw_comment_ = info_.comment;
    comment_->setToolTip(raw_comment_);
    info_layout->addWidget(comment_);
    UpdateCommentElide();
  }
}

AppDrawerSearchItem::~AppDrawerSearchItem() {
  LOG(INFO) << absl::StrCat("AppDrawerSearchItem is being deleted.");
}

void AppDrawerSearchItem::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    Launch();
    event->accept();
    return;
  }

  QWidget::mousePressEvent(event);
}

void AppDrawerSearchItem::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  UpdateCommentElide();
}

void AppDrawerSearchItem::showEvent(QShowEvent *event) {
  QWidget::showEvent(event);
  UpdateCommentElide();
}

void AppDrawerSearchItem::UpdateCommentElide() {
  if (comment_ == nullptr || raw_comment_.isEmpty()) {
    return;
  }

  const auto *layout_gen = qobject_cast<QHBoxLayout*>(layout());
  int available_width = comment_->width() * 0.8;
  if (layout_gen != nullptr) {
    const QMargins margins = layout_gen->contentsMargins();
    available_width = this->width() - margins.left() - margins.right() -
      btn_->width() - layout_gen->spacing();
    available_width *= 0.95;
  }

  if (available_width <= 0) {
    comment_->setText(raw_comment_);
    return;
  }

  const QFontMetrics metrics(comment_->font());
  comment_->setText(metrics.elidedText(raw_comment_, Qt::ElideRight,
    available_width));
}

void AppDrawerSearchItem::Launch() {
  LOG(INFO) << absl::StrFormat("Launching application: %s",
    info_.name.toStdString());

  if (info_.exec.isEmpty()) {
    LOG(ERROR) << absl::StrCat(
      "No executable command found for application: ",
      info_.name.toStdString(), ". Launch aborted.");
    return;
  }

  QString raw_exec = info_.exec;
  static const QRegularExpression field_regex(
    QStringLiteral("\\s%[fFuUdDnNickvm]"));
  QString clean_exec = raw_exec.replace(field_regex, QString());

  std::string cmd = "(" + clean_exec.toStdString() + ") &";
  int state = std::system(cmd.c_str());
  if (state != 0) {
    LOG(ERROR) << absl::StrCat(
      "Failed to launch application: ", info_.name.toStdString(),
      " with command: ", clean_exec.toStdString());
  }
}

void AppDrawerSearchItem::UpdateFilter(const QString& keyword) {
  bool should_show = false;
  if (info_.name.contains(keyword, Qt::CaseInsensitive)) {
    should_show = true;
  } else if (info_.generic_name.contains(keyword, Qt::CaseInsensitive)) {
    should_show = true;
  } else if (info_.comment.contains(keyword, Qt::CaseInsensitive)) {
    should_show = true;
  }

  if (should_show) {
    if (!this->isVisible()) {
      this->setVisible(true);
    }
  } else {
    if (this->isVisible()) {
      this->setVisible(false);
    }
  }
}

}  // namespace frontend
}  // namespace panel
