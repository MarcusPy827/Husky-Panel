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

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/components/app_indicator/app_indicator.h"

namespace panel {
namespace frontend {

AppIndicator::AppIndicator(QObject *parent) : QObject(parent) {
  LOG(INFO) << absl::StrCat("Initializing AppIndicator...");
  current_window_info_ = backend::InitCurrentWindowInfoServer(this);
  app_name_ = current_window_info_->GetApplicationName();
  package_name_ = current_window_info_->GetPackageName();
  connect(current_window_info_.get(),
    &backend::CurrentWindowProvider::CurrentWindowChanged,
    this, &AppIndicator::OnCurrentWindowChanged);
  LOG(INFO) << absl::StrCat("AppIndicator should be up!!");
}

AppIndicator::~AppIndicator() {
  current_window_info_ = nullptr;
  LOG(INFO) << absl::StrCat("AppIndicatorProvider is being deleted.");
}

QString AppIndicator::GetAppName() const {
  return app_name_;
}

QString AppIndicator::GetPackageName() const {
  return package_name_;
}

void AppIndicator::OnCurrentWindowChanged() {
  LOG(INFO) << absl::StrCat("Current window changed, updating AppIndicator...");
  app_name_ = current_window_info_->GetApplicationName();
  package_name_ = current_window_info_->GetPackageName();
  emit DataChanged();
}

}  // namespace frontend
}  // namespace panel
