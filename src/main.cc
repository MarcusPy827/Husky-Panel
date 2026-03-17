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
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/info_server/tray_handler/tray_def.h"
#include "src/mainwindow/mainwindow.h"
#include "src/theme_loader/quick_theme_provider.h"

#ifdef HUSKY_USE_VENDORED_LAYERSHELLQT
#include "lib/3rdparty/layer-shell-qt/src/interfaces/window.h"
#else
#include <LayerShellQt/window.h>
#endif

int main(int argc, char *argv[]) {
  LOG(INFO) << absl::StrCat("Initializing SNI types...");
  panel::backend::InitSystemTrayTypes();

  LOG(INFO) << absl::StrCat("Now initializing panel...");
  QGuiApplication a(argc, argv);

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

  LOG(INFO) << absl::StrCat("Initializing QML scheme provider...");
  auto* theme = new panel::loader::QuickThemeProvider(&a);

  LOG(INFO) << absl::StrCat("Loading panel window...");
  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("Theme", theme);

  engine.load(QUrl(QStringLiteral("qrc:/ui/interfaces/MainWindow.qml")));
  if (engine.rootObjects().isEmpty()) {
    LOG(ERROR) << absl::StrCat("Failed to load main panel QML root object.");
    return -1;
  }

  QObject* root = engine.rootObjects().first();
  QQuickWindow* window = qobject_cast<QQuickWindow*>(root);
  if (window) {
    QWindow* native = window;
    LayerShellQt::Window* layer_shell = LayerShellQt::Window::get(native);
    layer_shell->setAnchors(static_cast<LayerShellQt::Window::Anchor>(
      LayerShellQt::Window::Anchor::AnchorTop |
      LayerShellQt::Window::Anchor::AnchorLeft |
      LayerShellQt::Window::Anchor::AnchorRight));
    layer_shell->setLayer(LayerShellQt::Window::LayerTop);
    layer_shell->setKeyboardInteractivity(
      LayerShellQt::Window::KeyboardInteractivityNone);
    layer_shell->setExclusiveZone(32);
    window->show();
  } else {
    LOG(ERROR) << absl::StrCat("Failed to get QQuickWindow. ",
      "Please check your session.");
    return -1;
  }

  // Depreciated: Old Panel
  /*
  LOG(INFO) << absl::StrCat("Loading panel window...");
  panel::frontend::MainWindow w;
  w.show();
  */
  return a.exec();
}
