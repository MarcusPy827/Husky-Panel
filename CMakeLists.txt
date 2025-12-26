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
  NAMES Qt6 Qt5 
  REQUIRED COMPONENTS 
  Widgets 
  LinguistTools
  WaylandClient
  DBus
)

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

set(PROJECT_SOURCES
  ${TS_FILES}
  src/main.cc
  src/mainwindow/mainwindow.cc
  src/mainwindow/mainwindow.h
  src/mainwindow/mainwindow.ui
  src/user_info/user_info.h
  src/user_info/user_info.cc
  src/application_services/application_services.h
  src/application_services/application_services.cc
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

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(HuskyPanel
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
  )
  
  qt_create_translation(QM_FILES 
    ${CMAKE_SOURCE_DIR} 
    ${TS_FILES}
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

target_link_libraries(HuskyPanel 
  PRIVATE 
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::WaylandClient
  Qt${QT_VERSION_MAJOR}::DBus
  LayerShellQtInterface
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

target_include_directories(HuskyPanel PRIVATE ${PROJECT_SOURCE_DIR})
