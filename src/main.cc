/*
 * Copyright (C) 2025 MarcusPy827
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

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/tray_handler/tray_def.h"
#include "src/mainwindow/mainwindow.h"

int main(int argc, char *argv[]) {
  LOG(INFO) << absl::StrCat("Initializing SNI types...");
  panel::backend::InitSystemTrayTypes();

  LOG(INFO) << absl::StrCat("Now initializing panel...");
  QApplication a(argc, argv);

  LOG(INFO) << absl::StrCat("Loading locales...");
  QTranslator translator;
  const QStringList ui_lang = QLocale::system().uiLanguages();
  for (const QString &locale : ui_lang) {
    const QString base_name = "HuskyPanel_" + QLocale(locale).name();
    QString path = ":/translations/translations/"
      + base_name;
    if (translator.load(base_name, ":/translations/translations/")) {
      LOG(INFO) << absl::StrCat("Successfully loaded translation: ",
        base_name.toStdString(), ".");
      a.installTranslator(&translator);
      break;
    } else {
      LOG(WARNING) << absl::StrCat("Failed to load translation file '",
        base_name.toStdString(), "' at '", path.toStdString(), "'.");
    }
  }

  LOG(INFO) << absl::StrCat("Loading panel window...");
  panel::frontend::MainWindow w;
  w.show();
  return a.exec();
}
