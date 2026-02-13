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

#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QStackedWidget>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/components/app_indicator/app_indicator.h"

namespace panel {
namespace frontend {

AppIndicator::AppIndicator(QWidget *parent) : QWidget(parent) {
  QVBoxLayout * layout_gen = new QVBoxLayout();
  layout_gen->setContentsMargins(0, 4, 0, 4);
  layout_gen->setSpacing(0);
  setLayout(layout_gen);

  if (app_package_name_ == nullptr) {
    app_package_name_ = new QLabel();
    app_package_name_->setProperty("class", "app_indicator_package_name_desc");
  }
  layout_gen->addWidget(app_package_name_);

  if (app_name_ == nullptr) {
    app_name_ = new QLabel();
    app_name_->setProperty("class", "app_indicator_app_name_desc");
  }
  layout_gen->addWidget(app_name_);

  if (current_window_info_ == nullptr) {
    current_window_info_ = backend::InitCurrentWindowInfoServer(this);
    app_name_->setText(current_window_info_->GetApplicationName());
    app_package_name_->setText(current_window_info_->GetPackageName());
    connect(current_window_info_.get(), SIGNAL(CurrentWindowChanged()), this,
      SLOT(OnCurrentWindowChanged()));
  }
}

AppIndicator::~AppIndicator() {
  if (current_window_info_ != nullptr) {
    current_window_info_ = nullptr;
  }

  LOG(INFO) << absl::StrCat("AppIndicator is being deleted.");
}

void AppIndicator::OnCurrentWindowChanged() {
  LOG(INFO) << absl::StrCat("Current window has been changed, ",
    "updating AppIndicator labels...");
  app_name_->setText(current_window_info_->GetApplicationName());
  app_package_name_->setText(current_window_info_->GetPackageName());
}

}  // namespace frontend
}  // namespace panel
