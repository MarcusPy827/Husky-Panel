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

#include <string>

#include <QProcess>
#include <QRegularExpression>
#include <QStringList>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

#include "src/components/app_drawer/app_launcher.h"

namespace panel {
namespace frontend {

AppLauncher::AppLauncher(QObject* parent) : QObject(parent) {
  LOG(INFO) << absl::StrCat("AppLauncher constructer invoked!!");
}

void AppLauncher::launch(const QString& exec) {
  if (exec.isEmpty()) {
    LOG(ERROR) << absl::StrCat("AppLauncher: empty exec string, aborting...");
    return;
  }

  static const QRegularExpression field_regex(QStringLiteral(
    "\\s%[fFuUdDnNickvm]"));
  QString clean_exec = exec;
  clean_exec.replace(field_regex, QString());

  LOG(INFO) << absl::StrFormat("Now launching \"%s\"",
    clean_exec.toStdString());

  QStringList args = {QStringLiteral("-c"), clean_exec};
  bool ok = QProcess::startDetached(QStringLiteral("/bin/sh"), args);

  if (!ok) {
    LOG(ERROR) << absl::StrFormat("Failed launching \"%s\"",
      clean_exec.toStdString());
  }
}

}  // namespace frontend
}  // namespace panel
