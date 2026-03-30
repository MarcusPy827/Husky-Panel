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
#include <QQuickImageProvider>
#include <QIcon>
#include <QRegion>
#include <QtQml>
#include <QWindow>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/components/app_indicator/app_indicator.h"
#include "src/font_loader/font_loader.h"
#include "src/components/app_drawer/app_info_wrapper.h"
#include "src/components/app_drawer/app_launcher.h"
#include "src/components/krunner_btn/krunner_btn.h"
#include "src/info_server/power_options/power_options.h"
#include "src/info_server/session_handler/session_handler.h"
#include "src/info_server/user_info/user_info.h"
#include "src/info_server/tray_handler/tray_def.h"
#include "src/theme_loader/quick_theme_provider.h"
#include "src/translation_loader/translation_loader.h"
#include "src/utils/misc/misc.h"
#include "src/utils/xdg_icon_handler/xdg_icon_handler.h"
#include "src/utils/user_avatar_handler/user_avatar_handler.h"
#include "src/utils/layer_shell_helper/layer_shell_helper.h"

#ifdef HUSKY_USE_VENDORED_LAYERSHELLQT
#include "lib/3rdparty/layer-shell-qt/src/interfaces/window.h"
#else
#include <LayerShellQt/window.h>
#endif

void InjectEngineContext(QGuiApplication& application,
    QQmlApplicationEngine& target) {
  LOG(INFO) << absl::StrCat("Injecting context properties into QML engine...");

  LOG(INFO) << absl::StrCat("Initializing QML scheme provider...");
  auto* theme = new panel::loader::QuickThemeProvider(&application);
  target.rootContext()->setContextProperty("Theme", theme);
  LOG(INFO) << absl::StrCat("Successfully injected QML theme provider.");

  LOG(INFO) << absl::StrCat("Initializing translator via the class \"",
    "TranslationLoader\"...");
  LOG(INFO) << absl::StrCat("Initializing translator for status bar...");
  auto* translation_loader_bar = new panel::loader::TranslationLoader(
    ":/translations/translations/bar.locale",
    panel::loader::LanguageType::EN_US);
  target.rootContext()->setContextProperty("StatusBarTranslator",
    translation_loader_bar);
  LOG(INFO) << absl::StrCat("Successfully injected status bar translator.");

  LOG(INFO) << absl::StrCat("Initializing translator for application ",
    "drawer...");
  auto* translation_loader_drawer = new panel::loader::TranslationLoader(
    ":/translations/translations/app_drawer.locale",
    panel::loader::LanguageType::EN_US);
  target.rootContext()->setContextProperty("DrawerTranslator",
    translation_loader_drawer);
  LOG(INFO) << absl::StrCat("Successfully injected application drawer ",
    "translator.");

  LOG(INFO) << absl::StrCat("Initializing translator for calendar...");
  auto* translation_loader_calendar = new panel::loader::TranslationLoader(
    ":/translations/translations/calendar.locale",
    panel::loader::LanguageType::EN_US);
  target.rootContext()->setContextProperty("CalendarTranslator",
    translation_loader_calendar);
  LOG(INFO) << absl::StrCat("Successfully injected calendar translator.");
  LOG(INFO) << absl::StrCat("All translator loaders were successfully ",
    "injected!");

  LOG(INFO) << absl::StrCat("Initializing QML user info wrapper...");
  auto* user_info = new panel::backend::UserInfo(&application);
  target.rootContext()->setContextProperty("UserInfoProvider", user_info);
  LOG(INFO) << absl::StrCat("Successfully injected user info wrapper!!");

  LOG(INFO) << absl::StrCat("Initializing QML session handler...");
  auto* session_handler = new panel::backend::SessionHandler(&application);
  target.rootContext()->setContextProperty("SessionProvider", session_handler);
  LOG(INFO) << absl::StrCat("Successfully injected session handler!!");

  LOG(INFO) << absl::StrCat("Initializing QML power options handler...");
  auto* power_options = new panel::backend::PowerOptions(&application);
  target.rootContext()->setContextProperty("PowerProvider", power_options);
  LOG(INFO) << absl::StrCat("Successfully injected power options handler!!");

  LOG(INFO) << absl::StrCat("Initializing QML application info wrapper...");
  auto* app_info_wrapper = new panel::frontend::AppInfoWrapper(&application);
  target.rootContext()->setContextProperty("AppProvider", app_info_wrapper);
  LOG(INFO) << absl::StrCat(
    "Successfully injected application info wrapper!!");

  LOG(INFO) << absl::StrCat("Initializing QML application launcher...");
  auto* app_launcher = new panel::frontend::AppLauncher(&application);
  target.rootContext()->setContextProperty("AppLaunchProvider", app_launcher);
  LOG(INFO) << absl::StrCat("Successfully injected application launcher!!");

  LOG(INFO) << absl::StrCat("Initializing QML app indicator...");
  auto* current_window = new panel::frontend::AppIndicator(&application);
  target.rootContext()->setContextProperty("CurrentWindow", current_window);
  LOG(INFO) << absl::StrCat("Successfully injected app indicator!!");

  LOG(INFO) << absl::StrCat("Initializing KRunner toggler...");
  auto* krunner_btn = new panel::frontend::KRunnerBtn(&application);
  target.rootContext()->setContextProperty("KRunnerToggler", krunner_btn);
  LOG(INFO) << absl::StrCat("Successfully injected KRunner toggler!!");
}

void InjectImageProviders(QQmlApplicationEngine& target) {
  LOG(INFO) << absl::StrCat("Injecting image handlers into QML engine...");

  LOG(INFO) << absl::StrCat("Initializing XDG icon handler...");
  target.addImageProvider("icon", new panel::utils::XdgIconHandler());
  LOG(INFO) << absl::StrCat("Successfully injected XDG icon handler!!");

  LOG(INFO) << absl::StrCat("Initializing user avatar image handler...");
  target.addImageProvider("useravatar", new panel::utils::UserAvatarHandler());
  LOG(INFO) << absl::StrCat(
    "Successfully injected user avatar image handler!!");

  LOG(INFO) << absl::StrCat("Successfully injected all image hendlers!!");
}

void InitializedDepreciatedTranslator(QGuiApplication& application) {
  LOG(INFO) << absl::StrCat("Loading locales via QTranslator...");
  LOG(WARNING) << absl::StrCat("For this application, translation via ",
    "QTranslator is currently being depreciated, please consider using the \"",
    "TranslationLoader\" class if you want to add a new translation.");

  QTranslator translator;
  const QStringList ui_lang = QLocale::system().uiLanguages();
  for (const QString &locale : ui_lang) {
    const QString base_name = "HuskyPanel_" + QLocale(locale).name();
    QString path = ":/translations/translations/"
      + base_name;
    if (translator.load(base_name, ":/translations/translations/")) {
      LOG(INFO) << absl::StrCat("Successfully loaded translation: ",
        base_name.toStdString(), ".");
      application.installTranslator(&translator);
      break;
    } else {
      LOG(WARNING) << absl::StrCat("Failed to load translation file '",
        base_name.toStdString(), "' at '", path.toStdString(), "'.");
    }
  }

  LOG(INFO) << absl::StrCat("Done loading translations via QTranslator.");
}

void LoadIconFonts() {
  panel::loader::FontLoader::GetRoundedMaterialSymbolFont();
}

int main(int argc, char *argv[]) {
  LOG(INFO) << absl::StrCat("Initializing SNI types...");
  panel::backend::InitSystemTrayTypes();

  LOG(INFO) << absl::StrCat("Now initializing panel...");
  QGuiApplication a(argc, argv);
  InitializedDepreciatedTranslator(a);

  LOG(INFO) << absl::StrCat("Loading fonts...");
  LoadIconFonts();

  LOG(INFO) << absl::StrCat("Loading panel window...");
  QQmlApplicationEngine engine;
  InjectImageProviders(engine);
  InjectEngineContext(a, engine);

  LOG(INFO) << absl::StrCat("Initializing layer shell helper...");
  auto* layer_shell_helper = new panel::utils::LayerShellHelper(&a);
  engine.rootContext()->setContextProperty(
    "LayerShellHelper", layer_shell_helper);

  LOG(INFO) << absl::StrCat("Loading main QML file...");
  engine.load(QUrl(QStringLiteral("qrc:/ui/interfaces/MainWindow.qml")));
  if (engine.rootObjects().isEmpty()) {
    LOG(ERROR) << absl::StrCat("Failed to load main panel QML root object.");
    return -1;
  }

  LOG(INFO) << absl::StrCat("Now setting up layer shell for the panel window...");
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
      LayerShellQt::Window::KeyboardInteractivityOnDemand);
    layer_shell->setExclusiveZone(32);
    window->show();
  } else {
    LOG(ERROR) << absl::StrCat("Failed to get QQuickWindow. ",
      "Please check your session.");
    return -1;
  }

  return a.exec();
}
