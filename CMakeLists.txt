# Note: Please, do use Qt6 and consider only support Plasma 6. We did not test 
#       with Qt5 and Plasma 5, and please do not try to build with Qt5 while 
#       you are targeting Plasma 6.

cmake_minimum_required(VERSION 3.16)

project(HuskyPanel VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig 
  REQUIRED
)

pkg_check_modules(WaylandClient 
  REQUIRED 
  wayland-client 
  IMPORTED_TARGET
)

find_package(QT 
  NAMES Qt6 
  REQUIRED COMPONENTS 
  Widgets 
  LinguistTools
  WaylandClient
  DBus
)

find_package(
  KF6Service 
  REQUIRED
)

add_subdirectory(lib/3rdparty/abseil-cpp)
add_subdirectory(lib/3rdparty/gtest)
add_subdirectory(lib/3rdparty/qwindowkit)
add_subdirectory(lib/3rdparty/libdbusmenu)
add_subdirectory(cmake/material-color-utils)

find_package(Qt${QT_VERSION_MAJOR} 
  REQUIRED COMPONENTS 
  Widgets 
  LinguistTools
  WaylandClient
  DBus
)

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
  src/font_loader/font_loader.h
  src/font_loader/font_loader.cc
)

set(WIDGET_SOURCES
  src/mainwindow/mainwindow.cc
  src/mainwindow/mainwindow.h
)

set(UTIL_SOURCES
  src/utils/dbus_def.h
)

set(BACKEND_SOURCES
  src/application_services/application_services.h
  src/application_services/application_services.cc
  src/info_server/current_window/current_window_provider.h
  src/info_server/current_window/current_window_provider_factory.h
  src/info_server/current_window/current_window_provider_factory.cc
  src/info_server/current_window/current_window_kwin_impl.h
  src/info_server/current_window/current_window_kwin_impl.cc
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
  src/info_server/tray_handler/tray_handler.h
  src/info_server/tray_handler/tray_handler.cc
)

set(FRONTEND_SOURCES
  src/components/app_indicator/app_indicator_style.h
  src/components/app_indicator/app_indicator.h
  src/components/app_indicator/app_indicator.cc
  src/components/clock_btn/clock_btn.h
  src/components/clock_btn/clock_btn.cc
  src/components/app_drawer/app_drawer_btn.h
  src/components/app_drawer/app_drawer_btn.cc
  src/components/krunner_btn/krunner_btn.h
  src/components/krunner_btn/krunner_btn.cc
  src/components/battery_indicator/battery_indicator.h
  src/components/battery_indicator/battery_indicator.cc
  src/components/wlan_indicator/wlan_indicator.h
  src/components/wlan_indicator/wlan_indicator.cc
)

set(COMMON_SOURCES
  ${TS_FILES}
  ${MATERIAL_DESIGN_DEPS}
  ${WIDGET_SOURCES}
  ${UTIL_SOURCES}
  ${BACKEND_SOURCES}
  ${FRONTEND_SOURCES}
)

set(PROJECT_SOURCES
  ${COMMON_SOURCES}
  src/main.cc
)

# Switch off testing to accelerate 3rdparty builds
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(OLD_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

# No, we need to expclitly prevent layershell-qt from installing to system 
# paths to avoid possible ABI mismatches...
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dummy_install_path")
add_subdirectory(lib/3rdparty/layer-shell-qt EXCLUDE_FROM_ALL)
set(CMAKE_INSTALL_PREFIX ${OLD_INSTALL_PREFIX})

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
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::WaylandClient
  Qt${QT_VERSION_MAJOR}::DBus
  KF6::Service
  absl::strings
  absl::flat_hash_map
  absl::raw_logging_internal
  absl::optional
  absl::log
  absl::log_initialize
  dbusmenu-lxqt
  LayerShellQtInterface
  MaterialColorUtils
)

target_link_libraries(HuskyPanel
  PUBLIC 
  QWindowKit::Widgets
)

include(GNUInstallDirs)
install(TARGETS 
  HuskyPanel
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

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
