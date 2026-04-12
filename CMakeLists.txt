# Note: Please, do use Qt6 and consider only support Plasma 6. We did not test 
#       with Qt5 and Plasma 5, and please do not try to build with Qt5 while 
#       you are targeting Plasma 6.

cmake_minimum_required(VERSION 3.16)

project(HuskyPanel VERSION 0.9.4 LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF)
set(ABSL_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
# set(CMAKE_SKIP_INSTALL_RULES TRUE)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

option(USE_VENDORED_LIBS
  "Use vendored dbusmenu-lxqt and layer-shell-qt instead of system packages. \
   (abseil, gtest, material-color-utils, qwindowkit are always vendored.)"
  OFF
)

# The vendored layer-shell-qt requires a newer ECM than most non-rolling
# distros ship. When USE_VENDORED_LIBS is ON we prepend the locally-built ECM
# so that find_package(ECM) picks it up; otherwise the system ECM suffices.
if(USE_VENDORED_LIBS)
  list(PREPEND CMAKE_PREFIX_PATH
      ${CMAKE_BINARY_DIR}/ecm-install
  )
endif()

find_package(PkgConfig 
  REQUIRED
)

pkg_check_modules(WaylandClient
  REQUIRED
  wayland-client
  IMPORTED_TARGET
)

pkg_check_modules(XCB xcb IMPORTED_TARGET)

pkg_check_modules(PULSE REQUIRED libpulse IMPORTED_TARGET)

find_package(QT 
  NAMES Qt6 
  REQUIRED COMPONENTS 
  Widgets 
  Quick 
  LinguistTools
  WaylandClient
  DBus
)

find_package(
  KF6Service 
  REQUIRED
)

find_package(Qt${QT_VERSION_MAJOR} 
  REQUIRED COMPONENTS 
  Widgets 
  Quick 
  LinguistTools
  WaylandClient
  DBus
)

find_package(ECM REQUIRED NO_MODULE)

add_subdirectory(lib/3rdparty/abseil-cpp EXCLUDE_FROM_ALL)
add_subdirectory(lib/3rdparty/gtest EXCLUDE_FROM_ALL)

# Force QWindowKit static build
set(QWINDOWKIT_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(QWINDOWKIT_BUILD_STATIC ON)
set(QWINDOWKIT_BUILD_QUICK ON CACHE BOOL "" FORCE)
set(QWINDOWKIT_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/3rdparty/qwindowkit EXCLUDE_FROM_ALL)

add_subdirectory(cmake/material-color-utils EXCLUDE_FROM_ALL)

# ---------- Switchable vendored / system libraries ----------
if(USE_VENDORED_LIBS)
  # Force dbusmenu-lxqt static by wrapper subdirectory
  add_subdirectory(cmake/libdbusmenu)
endif()

if(TARGET PkgConfig::WaylandClient AND NOT TARGET Wayland::Client)
  add_library(Wayland::Client 
    INTERFACE 
    IMPORTED
  )

  target_link_libraries(Wayland::Client 
    INTERFACE 
    PkgConfig::WaylandClient
  )
endif()

set(TS_FILES 
  locales/HuskyPanel_zh_CN.ts
)

set(MATERIAL_DESIGN_DEPS
  res/resources.qrc
  src/utils/log/log.h
  src/utils/log/log.cc
  src/utils/strings/strings.h
  src/utils/strings/strings.cc
  src/utils/colors/colors.h
  src/utils/colors/colors.cc
  src/utils/color_palette_wrapper/color_palette_wrapper.h
  src/utils/color_palette_wrapper/color_palette_wrapper.cc
  src/theme_loader/theme_conf.h
  src/theme_loader/theme_loader.h
  src/theme_loader/theme_loader.cc
  src/theme_loader/quick_theme_provider.h
  src/theme_loader/quick_theme_provider.cc
  src/font_loader/font_loader.h
  src/font_loader/font_loader.cc
)

set(WIDGET_SOURCES
  # SHouldn't be any here as we migrate to QML
)

set(UTIL_SOURCES
  src/utils/dbus_def.h
  src/utils/misc/misc.h
  src/utils/misc/misc.cc
  src/utils/xdg_icon_handler/xdg_icon_handler.h
  src/utils/xdg_icon_handler/xdg_icon_handler.cc
  src/utils/user_avatar_handler/user_avatar_handler.h
  src/utils/user_avatar_handler/user_avatar_handler.cc
  src/utils/layer_shell_helper/layer_shell_helper.h
  src/utils/layer_shell_helper/layer_shell_helper.cc
  src/utils/layer_shell_helper/xorg_panel_helper.h
  src/utils/layer_shell_helper/xorg_panel_helper.cc
)

set(BACKEND_SOURCES
  src/application_services/application_services.h
  src/application_services/application_services.cc
  src/info_server/current_window/current_window_provider.h
  src/info_server/current_window/current_window_provider_factory.h
  src/info_server/current_window/current_window_provider_factory.cc
  src/info_server/current_window/current_window_kwin_impl.h
  src/info_server/current_window/current_window_kwin_impl.cc
  src/info_server/current_window/current_window_wlroots_impl.h
  src/info_server/current_window/current_window_wlroots_impl.cc
  src/info_server/current_window/current_window_xorg_impl.h
  src/info_server/current_window/current_window_xorg_impl.cc
  src/info_server/current_window/current_window_null_impl.h
  src/info_server/current_window/current_window_null_impl.cc
  src/info_server/clock/clock.h
  src/info_server/clock/clock.cc
  src/info_server/battery_info/battery_info.h
  src/info_server/battery_info/battery_info.cc
  src/info_server/wlan_info/wlan_info.h
  src/info_server/wlan_info/wlan_info.cc
  src/info_server/user_info/user_info.h
  src/info_server/user_info/user_info.cc
  src/info_server/tray_handler/tray_def.h
  src/info_server/tray_handler/tray_handler.h
  src/info_server/tray_handler/tray_handler.cc
  src/info_server/application_info/application_info.h
  src/info_server/application_info/application_info.cc
  src/info_server/power_options/power_options.h
  src/info_server/power_options/power_options.cc
  src/info_server/session_handler/session_handler.h
  src/info_server/session_handler/session_handler.cc
  src/info_server/volume_control/volume_control.h
  src/info_server/volume_control/volume_control.cc
)

set(FRONTEND_SOURCES
  src/components/app_indicator/app_indicator.h
  src/components/app_indicator/app_indicator.cc
  src/components/clock_btn/clock_btn.h
  src/components/clock_btn/clock_btn.cc
  src/components/app_drawer/app_info_wrapper.h
  src/components/app_drawer/app_info_wrapper.cc
  src/components/app_drawer/app_launcher.h
  src/components/app_drawer/app_launcher.cc
  src/components/krunner_btn/krunner_btn.h
  src/components/krunner_btn/krunner_btn.cc
  src/components/battery_indicator/battery_indicator.h
  src/components/battery_indicator/battery_indicator.cc
  src/components/wlan_indicator/wlan_indicator.h
  src/components/wlan_indicator/wlan_indicator.cc
  src/components/system_tray/tray_icon_item.h
  src/components/system_tray/tray_icon_item.cc
  src/components/system_tray/system_tray_handler.h
  src/components/system_tray/system_tray_handler.cc
  src/components/system_tray/tray_icon_image_handler.h
  src/components/system_tray/tray_icon_image_handler.cc
  src/components/calendar/calendar_style.h
  src/components/calendar/calendar.h
  src/components/calendar/calendar.cc
  src/components/volume_control/volume_control_handler.h
  src/components/volume_control/volume_control_handler.cc
)

set(CONFIG_LOADER_SOURCES
  src/config_loader/path_finder/path_finder.h
  src/config_loader/path_finder/path_finder.cc
  src/config_loader/system_tray/tray_config_handler.h
  src/config_loader/system_tray/tray_config_handler.cc
)

set(I18N_SOURCES
  src/translation_loader/translation_loader.h
  src/translation_loader/translation_loader.cc
)

set(COMMON_SOURCES
  ${TS_FILES}
  ${MATERIAL_DESIGN_DEPS}
  ${WIDGET_SOURCES}
  ${UTIL_SOURCES}
  ${BACKEND_SOURCES}
  ${FRONTEND_SOURCES}
  ${CONFIG_LOADER_SOURCES}
  ${I18N_SOURCES}
)

set(PROJECT_SOURCES
  ${COMMON_SOURCES}
  src/main.cc
)

if(USE_VENDORED_LIBS)
  message(STATUS "USE_VENDORED_LIBS=ON: building vendored dbusmenu-lxqt and layer-shell-qt")

  # Switch off testing to accelerate 3rdparty builds
  set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(OLD_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

  # No, we need to explicitly prevent layershell-qt from installing to system
  # paths to avoid possible ABI mismatches...
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dummy_install_path")
  add_subdirectory(lib/3rdparty/layer-shell-qt EXCLUDE_FROM_ALL)
  set(CMAKE_INSTALL_PREFIX ${OLD_INSTALL_PREFIX})

  set(VENDORED_STATIC_SWITCHABLE_LIBS  dbusmenu-lxqt LayerShellQtInterface)
  set(SYSTEM_DYNAMIC_SWITCHABLE_LIBS)

  # Let source files know to use vendored layer-shell-qt headers
  add_compile_definitions(HUSKY_USE_VENDORED_LAYERSHELLQT)
else()
  message(STATUS "USE_VENDORED_LIBS=OFF: using system dbusmenu-lxqt and LayerShellQt")
  find_package(dbusmenu-lxqt REQUIRED)
  find_package(LayerShellQt REQUIRED)

  set(VENDORED_STATIC_SWITCHABLE_LIBS)
  set(SYSTEM_DYNAMIC_SWITCHABLE_LIBS  dbusmenu-lxqt LayerShellQt::Interface)
endif()

set_source_files_properties(${TS_FILES}
  PROPERTIES OUTPUT_LOCATION "${CMAKE_BINARY_DIR}/locales"
)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/locales")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_create_translation(QM_FILES 
    ${CMAKE_SOURCE_DIR} 
    ${TS_FILES}
  )

  set_source_files_properties(${QM_FILES} 
    PROPERTIES 
    QT_RESOURCE_PREFIX 
    "/i18n"
  )

  qt_add_executable(HuskyPanel
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${QM_FILES}
    res/resources.qrc
  )

  qt6_generate_wayland_protocol_client_sources(HuskyPanel
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-foreign-toplevel-management-unstable-v1.xml
  )

  if(USE_VENDORED_LIBS)
    target_include_directories(HuskyPanel PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/lib/3rdparty/libdbusmenu/src"
      "${CMAKE_CURRENT_BINARY_DIR}/cmake/libdbusmenu/dbusmenu-build/src"
    )
  endif()
else()
  if(ANDROID)
    message(FATAL_ERROR "Ewwww... This will never work in Android...")

  else()
    add_executable(HuskyPanel
      ${PROJECT_SOURCES}
    )

  endif()

  qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

if(COMMAND qt_add_lupdate)
  qt_add_lupdate(HuskyPanel
    TS_FILES 
    ${TS_FILES}
  )
endif()

target_link_libraries(HuskyPanel 
  PRIVATE
  -Wl,-Bstatic
  absl::strings
  absl::flat_hash_map
  absl::raw_logging_internal
  absl::optional
  absl::log
  absl::log_initialize
  ${VENDORED_STATIC_SWITCHABLE_LIBS}
  MaterialColorUtils
  QWindowKit::Widgets
  QWindowKit::Quick

  -Wl,-Bdynamic
  ${SYSTEM_DYNAMIC_SWITCHABLE_LIBS}
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Quick
  Qt${QT_VERSION_MAJOR}::WaylandClient
  Qt${QT_VERSION_MAJOR}::DBus
  KF6::Service
  PkgConfig::PULSE
  $<$<BOOL:${XCB_FOUND}>:PkgConfig::XCB>
)

include(GNUInstallDirs)
install(TARGETS 
  HuskyPanel
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ---------- KWin script plugin ----------
option(INSTALL_KWIN_PLUGIN
  "Install the KWin app-bridge script plugin to the system kwin/scripts directory."
  ON
)

if(INSTALL_KWIN_PLUGIN)
  set(_KWIN_PLUGIN_ID "marcus-app-bridge")
  set(_KWIN_SCRIPTS_DIR "${CMAKE_INSTALL_DATADIR}/kwin/scripts/${_KWIN_PLUGIN_ID}")

  install(FILES
    plugins/kde/app-bridge/metadata.json
    DESTINATION "${_KWIN_SCRIPTS_DIR}"
  )
  install(DIRECTORY
    plugins/kde/app-bridge/contents
    DESTINATION "${_KWIN_SCRIPTS_DIR}"
  )
endif()

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(HuskyPanel)
endif()

include(CTest)
if(BUILD_TESTING)
  add_executable(HuskyPanelTest
    ${COMMON_SOURCES}
    src/utils/color_palette_wrapper/color_palette_wrapper_test.cc
  )
  target_link_libraries(HuskyPanelTest PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    QWindowKit::Widgets
    absl::base
    absl::log
    absl::log_initialize
    GTest::gtest_main
    MaterialColorUtils
  )

  include(GoogleTest)
  gtest_discover_tests(HuskyPanelTest)
endif()

target_include_directories(HuskyPanel PRIVATE ${PROJECT_SOURCE_DIR})
