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

  QWidget * container = new QWidget();
  internal_layout->addWidget(container);

  if (is_default) {
    container->setProperty("class", "app_drawer_side_pane_item_active");
  } else {
    container->setProperty("class", "app_drawer_side_pane_item");
  }

  QHBoxLayout * layout_gen = new QHBoxLayout();
  layout_gen->setContentsMargins(24, 0, 24, 0);
  layout_gen->setSpacing(12);
  layout_gen->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  container->setLayout(layout_gen);

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
  layout_gen->addWidget(icon_);

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
  layout_gen->addWidget(label_);
}

AppDrawerSidePaneItem::~AppDrawerSidePaneItem() {
  LOG(INFO) << absl::StrCat("AppDrawerSidePaneItem is being deleted.");
}

}  // namespace frontend
}  // namespace panel
