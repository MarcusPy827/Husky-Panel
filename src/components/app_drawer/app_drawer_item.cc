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

#include <string>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/components/app_drawer/app_drawer_item.h"

namespace panel {
namespace frontend {

AppDrawerItem::AppDrawerItem(const AppInfo& info,
    QWidget *parent) : QWidget(parent) {
  this->info_ = info;
  this->setProperty("class", "app_drawer_item");
  this->setMinimumHeight(128);

  QVBoxLayout * layout_gen = new QVBoxLayout;
  layout_gen->setContentsMargins(0, 0, 0, 0);
  layout_gen->setSpacing(6);
  layout_gen->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  this->setLayout(layout_gen);

  QHBoxLayout * btn_layout = new QHBoxLayout;
  btn_layout->setContentsMargins(0, 0, 0, 0);
  btn_layout->setSpacing(0);
  layout_gen->addLayout(btn_layout);

  QSpacerItem * left_spacer = new QSpacerItem(8, 64, QSizePolicy::Expanding,
    QSizePolicy::Fixed);
  btn_layout->addSpacerItem(left_spacer);

  if (btn_ == nullptr) {
    btn_ = new QToolButton();
    btn_->setProperty("class", "drawer_item_btn");
    btn_->setIconSize(QSize(64, 64));
    btn_->setMinimumSize(QSize(64, 64));
  }
  QObject::connect(btn_, &QToolButton::clicked, this, &AppDrawerItem::Launch);
  btn_->setIcon(QIcon::fromTheme(info_.icon_name));
  btn_layout->addWidget(btn_);

  QSpacerItem * right_spacer = new QSpacerItem(8, 64, QSizePolicy::Expanding,
    QSizePolicy::Fixed);
  btn_layout->addSpacerItem(right_spacer);

  if (title_ == nullptr) {
    title_ = new QLabel();
    title_->setProperty("class", "app_drawer_item_label");
  }
  title_->setText(info_.name);
  title_->setWordWrap(true);
  title_->setMaximumWidth(128);
  title_->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  layout_gen->addWidget(title_);
}

AppDrawerItem::~AppDrawerItem() {
  LOG(INFO) << absl::StrCat("AppDrawerItem is being deleted.");
}

void AppDrawerItem::Launch() {
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

  std::string cmd = "(" + info_.exec.toStdString() + ") &";
  int state = std::system(cmd.c_str());
  if (state != 0) {
    LOG(ERROR) << absl::StrCat(
      "Failed to launch application: ", info_.name.toStdString(),
      " with command: ", clean_exec.toStdString());
  }
}

}  // namespace frontend
}  // namespace panel
