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
#include <QLabel>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/font_loader/font_loader.h"
#include "src/components/app_drawer/app_drawer_side_pane_item.h"

namespace panel {
namespace frontend {

AppDrawerSidePaneItem::AppDrawerSidePaneItem(QString icon, QString name,
    QString group_identifier, QString id, bool is_default, QWidget * parent) :
    QWidget(parent) {
  QHBoxLayout * internal_layout = new QHBoxLayout();
  internal_layout->setContentsMargins(0, 0, 0, 0);
  internal_layout->setSpacing(0);
  this->setLayout(internal_layout);

  if (container_ == nullptr) {
    container_ = new QWidget();
    container_->setProperty("class", "app_drawer_side_pane_item");
  }
  internal_layout->addWidget(container_);

  if (container_active_ == nullptr) {
    container_active_ = new QWidget();
    container_active_->setProperty("class", "app_drawer_side_pane_item_active");
  }
  internal_layout->addWidget(container_active_);

  if (layout_gen_ == nullptr) {
    layout_gen_ = new QHBoxLayout();
  }

  layout_gen_->setContentsMargins(24, 0, 24, 0);
  layout_gen_->setSpacing(12);
  layout_gen_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

  if (is_default) {
    container_->setVisible(false);
    container_active_->setVisible(true);
    container_active_->setLayout(layout_gen_);
  } else {
    container_active_->setVisible(false);
    container_->setVisible(true);
    container_->setLayout(layout_gen_);
  }

  if (icon_ == nullptr) {
    icon_ = new QLabel();
    if (is_default) {
      icon_->setProperty("class", "app_drawer_side_pane_item_icon_active");
    } else {
      icon_->setProperty("class", "app_drawer_side_pane_item_icon");
    }
  }

  icon_->setText(icon);
  icon_->setFont(loader::FontLoader::GetRoundedMaterialSymbolFont());
  icon_->setVisible(true);
  layout_gen_->addWidget(icon_);

  if (label_== nullptr) {
    label_ = new QLabel();
    if (is_default) {
      label_->setProperty("class", "app_drawer_side_pane_item_label_active");
    } else {
      label_->setProperty("class", "app_drawer_side_pane_item_label");
    }
  }
  label_->setText(name);
  label_->setVisible(true);
  layout_gen_->addWidget(label_);

  this->group_identifier_ = group_identifier;
  this->id_ = id;
}

AppDrawerSidePaneItem::~AppDrawerSidePaneItem() {
  LOG(INFO) << absl::StrCat("AppDrawerSidePaneItem is being deleted.");
}

void AppDrawerSidePaneItem::OnGroupUpdated(const QString& group_identifier,
    const QString& id) {
  if (group_identifier == this->group_identifier_ && id == this->id_) {
    container_->setVisible(false);
    container_active_->setVisible(true);
    container_active_->setLayout(layout_gen_);
  } else {
    container_active_->setVisible(false);
    container_->setVisible(true);
    container_->setLayout(layout_gen_);
  }
}

void AppDrawerSidePaneItem::mousePressEvent(QMouseEvent *event) {
  emit GroupSelected(this->group_identifier_, this->id_);
}

}  // namespace frontend
}  // namespace panel
