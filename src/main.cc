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
#include <QDir>
#include <QLibraryInfo>
#include <QQuickStyle>
#include <QIcon>
#include <QRegion>
#include <QtQml>
#include <QWindow>

#include "absl/log/initialize.h"
#include "absl/log/globals.h"
#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/components/app_indicator/app_indicator.h"
#include "src/components/battery_indicator/battery_indicator.h"
#include "src/components/wlan_indicator/wlan_indicator.h"
#include "src/font_loader/font_loader.h"
#include "src/components/app_drawer/app_info_wrapper.h"
#include "src/components/app_drawer/app_launcher.h"
#include "src/components/clock_btn/clock_btn.h"
#include "src/components/krunner_btn/krunner_btn.h"
#include "src/info_server/power_options/power_options.h"
#include "src/info_server/session_handler/session_handler.h"
#include "src/info_server/user_info/user_info.h"
#include "src/info_server/tray_handler/tray_def.h"
#include "src/components/system_tray/system_tray_handler.h"
#include "src/components/system_tray/tray_icon_image_handler.h"
#include "src/components/volume_control/volume_control_handler.h"
#include "src/components/bluetooth/bluetooth_handler.h"
#include "src/components/network_control/network_control_handler.h"
#include "src/theme_loader/quick_theme_provider.h"
#include "src/translation_loader/translation_loader.h"
#include "src/utils/misc/misc.h"
#include "src/utils/xdg_icon_handler/xdg_icon_handler.h"
#include "src/utils/user_avatar_handler/user_avatar_handler.h"
#include "src/utils/layer_shell_helper/layer_shell_helper.h"
#include "src/utils/layer_shell_helper/xorg_panel_helper.h"
#include "QWKQuick/qwkquickglobal.h"

#ifdef HUSKY_USE_VENDORED_LAYERSHELLQT
#include "lib/3rdparty/layer-shell-qt/src/interfaces/window.h"
#else
#include <LayerShellQt/window.h>
#endif

/**
 * @brief Registers all backend and frontend context objects into the QML engine.
 *
 * @details Instantiates and injects all translation loaders, info wrappers,
 *          component handlers, and utility objects as named context properties
 *          accessible from QML.
 * @param application (QGuiApplication&) The application instance used as
 *        parent for long-lived objects.
 * @param target (QQmlApplicationEngine&) The engine to inject context into.
 * @return void.
 */

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

  LOG(INFO) << absl::StrCat("Initializing translator for config panel...");
  auto* translation_loader_config_panel = new panel::loader::TranslationLoader(
    ":/translations/translations/config_panel.locale",
    panel::loader::LanguageType::EN_US);
  target.rootContext()->setContextProperty("ConfigPanelTranslator",
    translation_loader_config_panel);
  LOG(INFO) << absl::StrCat("Successfully injected config panel translator.");

  LOG(INFO) << absl::StrCat("Initializing translator for volume control...");
  auto* translation_loader_volume_control =
    new panel::loader::TranslationLoader(
      ":/translations/translations/volume_control.locale",
      panel::loader::LanguageType::EN_US);
  target.rootContext()->setContextProperty("VolumeControlTranslator",
    translation_loader_volume_control);
  LOG(INFO) << absl::StrCat("Successfully injected volume control translator.");

  LOG(INFO) << absl::StrCat("Initializing translator for bluetooth...");
  auto* translation_loader_bluetooth =
    new panel::loader::TranslationLoader(
      ":/translations/translations/bluetooth.locale",
      panel::loader::LanguageType::EN_US);
  target.rootContext()->setContextProperty("BluetoothTranslator",
    translation_loader_bluetooth);
  LOG(INFO) << absl::StrCat("Successfully injected bluetooth translator.");

  LOG(INFO) << absl::StrCat("Initializing translator for network...");
  auto* translation_loader_network_control =
    new panel::loader::TranslationLoader(
      ":/translations/translations/wlan_control.locale",
      panel::loader::LanguageType::EN_US);
  target.rootContext()->setContextProperty("NetworkTranslator",
    translation_loader_network_control);
  LOG(INFO) << absl::StrCat("Successfully injected network translator.");

  LOG(INFO) << absl::StrCat("Initializing translator for battery...");
  auto* translation_loader_battery =
    new panel::loader::TranslationLoader(
      ":/translations/translations/battery_control.locale",
      panel::loader::LanguageType::EN_US);
  target.rootContext()->setContextProperty("BatteryTranslator",
    translation_loader_battery);
  LOG(INFO) << absl::StrCat("Successfully injected battery translator.");

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

  LOG(INFO) << absl::StrCat("Initializing clock button...");
  auto* clock_btn = new panel::frontend::ClockBtn(nullptr, &application);
  target.rootContext()->setContextProperty("ClockProvider", clock_btn);
  LOG(INFO) << absl::StrCat("Successfully injected clock button!!");

  LOG(INFO) << absl::StrCat("Initializing KRunner toggler...");
  auto* krunner_btn = new panel::frontend::KRunnerBtn(&application);
  target.rootContext()->setContextProperty("KRunnerToggler", krunner_btn);
  LOG(INFO) << absl::StrCat("Successfully injected KRunner toggler!!");

  LOG(INFO) << absl::StrCat("Initializing battery handler...");
  auto* battery_indicator = new panel::frontend::BatteryIndicator(&application);
  target.rootContext()->setContextProperty("BatteryHandler",
    battery_indicator);
  LOG(INFO) << absl::StrCat("Successfully injected battery handler!!");

  LOG(INFO) << absl::StrCat("Initializing WLAN handler...");
  auto* wlan_indicator = new panel::frontend::WLANIndicator(&application);
  target.rootContext()->setContextProperty("WLANProvider", wlan_indicator);
  LOG(INFO) << absl::StrCat("Successfully injected WLAN handler!!");

  LOG(INFO) << absl::StrCat("Initializing system tray handler...");
  auto* tray_handler = new panel::frontend::SystemTrayHandler(&application);
  target.rootContext()->setContextProperty("TrayHandler", tray_handler);
  target.addImageProvider("trayicon",
    new panel::frontend::TrayIconImageHandler(tray_handler));
  LOG(INFO) << absl::StrCat("Successfully injected system tray handler!!");

  LOG(INFO) << absl::StrCat("Initializing volume control handler...");
  auto* volume_handler = new panel::frontend::VolumeControlHandler(
    &application);
  target.rootContext()->setContextProperty("VolumeHandler", volume_handler);
  LOG(INFO) << absl::StrCat("Successfully injected volume control handler!!");

  LOG(INFO) << absl::StrCat("Initializing bluetooth handler...");
  auto* bluetooth_handler = new panel::frontend::BluetoothHandler(
    &application);
  target.rootContext()->setContextProperty("BluetoothHandler",
    bluetooth_handler);
  LOG(INFO) << absl::StrCat("Successfully injected bluetooth handler!!");

  LOG(INFO) << absl::StrCat("Initializing network control handler...");
  auto* network_handler = new panel::frontend::NetworkControlHandler(
    &application);
  target.rootContext()->setContextProperty("NetworkHandler", network_handler);
  LOG(INFO) << absl::StrCat("Successfully injected network control handler!!");
}


/**
 * @brief Registers all custom QML image providers into the engine.
 *
 * @details Currently registers the XDG icon handler (@c "icon") and the user
 *          avatar handler (@c "useravatar").
 * @param target (QQmlApplicationEngine&) The engine to add image providers to.
 * @return void.
 */

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


/**
 * @brief Loads a Qt @c QTranslator for the current system locale.
 *
 * @details This translation path is being deprecated in favour of
 *          @c TranslationLoader. Iterates the system UI language list and
 *          attempts to load the first matching @c HuskyPanel_<locale> file.
 * @param application (QGuiApplication&) The application instance to install
 *        the translator into.
 * @note The method is depreciated, and do NOT add any new translations to the
 *       @c .ts file. For any new translations, please use the
 *       @c TranslationLoader class instead.
 * @deprecated So do NOT add any new translations to the @c .ts file.
 * @see The new @c TranslationLoader class.
 * @return void.
 */

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


/**
 * @brief Pre-loads the Material Symbols icon font.
 *
 * @see The existing @c panel::loader::FontLoader class.
 * @return void.
 */

void LoadIconFonts() {
  panel::loader::FontLoader::GetOutlinedMaterialSymbolFont();
  panel::loader::FontLoader::GetSharpMaterialSymbolFont();
  panel::loader::FontLoader::GetRoundedMaterialSymbolFont();
}


/**
 * @brief Pre-loads platform theme.
 * 
 * @details Initializes the platform theme by deteting whether the Breeze style
 *          is avaliable. If so, then set the style to Breeze, otherwise
 *          simply fallback to Basic.
 * @return void.
 */

void LoadPlatformTheme() {
  // First clear existing platform theme
  qputenv("QT_QPA_PLATFORMTHEME", "");

  // Setup new style
  const QString qml_root = QLibraryInfo::path(QLibraryInfo::QmlImportsPath);
  if (QDir(qml_root + "/QtQuick/Controls/Breeze").exists() ||
      QDir(qml_root + "/QtQuick/Controls.2/Breeze").exists()) {
    LOG(INFO) << absl::StrCat("Breeze style detected in Qt Quick Controls.");
    QQuickStyle::setStyle("Breeze");
    return;
  }

  LOG(WARNING) << absl::StrCat("No known platform style detected in Qt Quick,",
    " falling back to Basic.");
  QQuickStyle::setStyle("Basic");
}


/**
 * @brief Application entry point.
 *
 * @details Initializes logging, registers Qt meta-types, sets up the QML
 *          engine with all context properties and image providers, loads the
 *          main QML file, and configures the panel window for the current
 *          display session (Wayland layer-shell or X11/EWMH DOCK).
 * @param argc (int) Argument count.
 * @param argv (char**) Argument vector.
 * @return (int) Exit code.
 */

int main(int argc, char *argv[]) {
  // Initialize logging
  absl::InitializeLog();
  absl::SetMinLogLevel(absl::LogSeverityAtLeast::kInfo);

  LOG(INFO) << absl::StrCat("Loading platform theme...");
  LoadPlatformTheme();

  LOG(INFO) << absl::StrCat("Initializing SNI types...");
  panel::backend::InitSystemTrayTypes();

  LOG(INFO) << absl::StrCat("Now initializing panel...");
  QApplication a(argc, argv);

  LOG(WARNING) << absl::StrCat("Loading DEPRECIATED translator...");
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

  LOG(INFO) << absl::StrCat("Registering QWindowKit QML types...");
  QWK::registerTypes(&engine);

  LOG(INFO) << absl::StrCat("Loading main QML file...");
  engine.load(QUrl(QStringLiteral("qrc:/ui/interfaces/MainWindow.qml")));
  if (engine.rootObjects().isEmpty()) {
    LOG(ERROR) << absl::StrCat("Failed to load main panel QML root object.");
    return -1;
  }

  LOG(INFO) << absl::StrCat(
    "Now setting up panel window for the current display session...");
  QObject* root = engine.rootObjects().first();
  QQuickWindow* window = qobject_cast<QQuickWindow*>(root);
  if (!window) {
    LOG(ERROR) << absl::StrCat("Failed to get QQuickWindow. ",
      "Please check your session.");
    return -1;
  }

  QWindow* native = window;
  const bool is_x11 =
      QGuiApplication::platformName() == QLatin1String("xcb");

  if (is_x11) {
    LOG(INFO) << absl::StrCat("X11/Xorg session detected, now using EWMH ",
      "DOCK setup...");
    panel::utils::XOrgPanelHelper::SetupXorgPanelWindow(native, 32);
  } else {
    LOG(INFO) << absl::StrCat("Wayland session detected, using layer shell ",
      "setup...");
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
  }

  return a.exec();
}
