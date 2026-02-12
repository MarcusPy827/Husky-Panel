# Check: Either install abseil or include vendor/abseil-cpp
# as a subdirectory in ${CMAKE_SOURCE_DIR}/CMakeLists.txt.
# The version required is exactly lts_2025_08_14, DO NOT upgrade.
include_directories(${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities)

set(CMAKE_CXX_STANDARD 17)

# GTest (GUnit) is disabled here.
set(LIB_SOURCES
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/blend/blend.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/blend/blend.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/blend/blend_test.cc

  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/cam/cam.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/cam/cam.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/cam/cam_test.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/cam/hct.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/cam/hct.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/cam/hct_test.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/cam/hct_solver.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/cam/hct_solver.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/cam/hct_solver_test.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/cam/viewing_conditions.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/cam/viewing_conditions.cc

  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/contrast/contrast.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/contrast/contrast.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/contrast/contrast_test.cc

  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/dislike/dislike.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/dislike/dislike.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/dislike/dislike_test.cc

  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/dynamiccolor/contrast_curve.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/dynamiccolor/dynamic_color.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/dynamiccolor/dynamic_color.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/dynamiccolor/dynamic_scheme.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/dynamiccolor/dynamic_scheme.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/dynamiccolor/material_dynamic_colors.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/dynamiccolor/material_dynamic_colors.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/dynamiccolor/tone_delta_pair.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/dynamiccolor/variant.h

  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/palettes/core.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/palettes/tones.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/palettes/tones.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/palettes/tones_test.cc

  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/quantize/celebi.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/quantize/celebi.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/quantize/celebi_test.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/quantize/lab.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/quantize/lab.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/quantize/wsmeans.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/quantize/wsmeans.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/quantize/wsmeans_test.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/quantize/wu.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/quantize/wu.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/quantize/wu_test.cc

  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_content.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_content.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_expressive.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_expressive.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_fidelity.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_fidelity.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_fruit_salad.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_fruit_salad.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_monochrome.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_monochrome.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_monochrome_test.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_neutral.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_neutral.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_rainbow.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_rainbow.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_tonal_spot.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_tonal_spot.cc
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_vibrant.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/scheme/scheme_vibrant.cc

  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/score/score.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/score/score.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/score/score_test.cc

  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/temperature/temperature_cache.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/temperature/temperature_cache.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/temperature/temperature_cache_test.cc
  
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/utils/utils.h
  ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/utils/utils.cc
  # ${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities/cpp/utils/utils_test.cc
)

add_library(MaterialColorUtils STATIC ${LIB_SOURCES})

target_include_directories(MaterialColorUtils BEFORE PRIVATE
    "${CMAKE_SOURCE_DIR}/lib/3rdparty/abseil-cpp"
)

# Modfications, these are to ensure that the library could be built without
# errors and warnings and without modifying the original source files.
target_compile_options(MaterialColorUtils PRIVATE
  # Fixed: Original headers lacks include for optional causing build errors
  -include optional
    
  # Omitted: warning for using abs instead of std::abs
  -Wno-absolute-value
)

target_include_directories(MaterialColorUtils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/3rdparty/material-color-utilities>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS MaterialColorUtils
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
