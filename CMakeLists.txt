cmake_minimum_required(VERSION 3.16)

project(HuskyPanel VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES 
  locales/HuskyPanel_zh_CN.ts
)

set(PROJECT_SOURCES
  ${TS_FILES}
  src/main.cc
  src/mainwindow/mainwindow.cc
  src/mainwindow/mainwindow.h
  src/mainwindow/mainwindow.ui
)

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

target_link_libraries(HuskyPanel PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

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
