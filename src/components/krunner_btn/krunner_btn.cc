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

#include "src/components/krunner_btn/krunner_btn.h"
#include "src/application_services/application_services.h"

namespace panel {
namespace frontend {

KRunnerBtn::KRunnerBtn(QObject *parent) : QObject(parent) {
  LOG(INFO) << absl::StrCat("KRunnerBtn constructer invoked!!");
}

void KRunnerBtn::ToggleKRunner() {
  backend::ApplicationServices::GetKRunner();
}

void KRunnerBtn::toggleKRunner() {
  ToggleKRunner();
}

}  // namespace frontend
}  // namespace panel
