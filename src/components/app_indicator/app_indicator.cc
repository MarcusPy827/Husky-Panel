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

#include <QDebug>
#include <QGridLayout>
#include <QStackedWidget>

#include "src/components/app_indicator/app_indicator.h"

namespace panel {
namespace frontend {

AppIndicator::AppIndicator(QWidget *parent) : QWidget(parent) {
  QGridLayout * layout_gen = new QGridLayout();
  layout_gen->setContentsMargins(0, 0, 0, 0);
  layout_gen->setSpacing(0);
  setLayout(layout_gen);

  if (app_name_ == nullptr) {
    app_name_ = new QLabel();
    app_name_->setText(tr("INITIALIZING"));
    layout_gen->addWidget(app_name_, 0, 0);
  }

  if (app_package_name_ == nullptr) {
    app_package_name_ = new QLabel();
    app_package_name_->setText("APP INDICATOR");
    layout_gen->addWidget(app_package_name_, 1, 0);
  }

  if (current_window_info_ == nullptr) {
    current_window_info_ = backend::InitCurrentWindowInfoServer(this);
    app_name_->setText(current_window_info_->GetApplicationName());
    app_package_name_->setText(current_window_info_->GetPackageName());
    connect(current_window_info_.get(), SIGNAL(CurrentWindowChanged()), this,
      SLOT(OnCurrentWindowChanged()));
  }

  app_name_->setStyleSheet("color: white;");
  app_package_name_->setStyleSheet("color: white;");
}

AppIndicator::~AppIndicator() {
  if (current_window_info_ != nullptr) {
    current_window_info_ = nullptr;
  }

  qInfo() << "[INFO] AppIndicator: AppIndicator is being deleted";
}

void AppIndicator::OnCurrentWindowChanged() {
  qInfo() << "[INFO] AppIndicator: Current window changed,"
    << "updating app indicator labels.";
  app_name_->setText(current_window_info_->GetApplicationName());
  app_package_name_->setText(current_window_info_->GetPackageName());
}

}  // namespace frontend
}  // namespace panel
