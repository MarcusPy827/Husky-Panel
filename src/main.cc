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

#include "src/mainwindow/mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QTranslator translator;
  const QStringList ui_lang = QLocale::system().uiLanguages();
  for (const QString &locale : ui_lang) {
    const QString base_name = "HuskyPanel_" + QLocale(locale).name();
    QString path = ":/translations/translations/"
      + base_name;
    if (translator.load(base_name, ":/translations/translations/")) {
      qInfo() << "[ OK ] Translator: Successfully loaded translation:"
        << base_name;
      a.installTranslator(&translator);
      break;
    } else {
      qWarning() << "[WARN] Translator: Failed to load"
        << base_name << "at" << QCoreApplication::applicationDirPath()
        + "/locales/";
    }
  }

  panel::frontend::MainWindow w;
  w.show();
  return a.exec();
}
