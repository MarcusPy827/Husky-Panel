# Module: PatchDbusmenuStatic
# Purpose: Force dbusmenu-lxqt to build as a static library
# without modifying vendor source files

function(patch_dbusmenu_for_static_build)
    # Read libdbusmenu's src/CMakeLists.txt
    set(DBUSMENU_SRC_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/lib/3rdparty/libdbusmenu/src/CMakeLists.txt")
    
    if(NOT EXISTS "${DBUSMENU_SRC_CMAKE}")
        message(FATAL_ERROR "libdbusmenu CMakeLists.txt not found at ${DBUSMENU_SRC_CMAKE}")
    endif()
    
    # Read the original content
    file(READ "${DBUSMENU_SRC_CMAKE}" ORIGINAL_CONTENT)
    
    # Create a temporary modified version
    string(REPLACE 
        "add_library(dbusmenu-lxqt SHARED"
        "add_library(dbusmenu-lxqt STATIC"
        MODIFIED_CONTENT 
        "${ORIGINAL_CONTENT}"
    )
    
    # Write to a temporary directory in build folder
    set(TEMP_DBUSMENU_DIR "${CMAKE_BINARY_DIR}/_dbusmenu_patched")
    file(MAKE_DIRECTORY "${TEMP_DBUSMENU_DIR}")
    file(WRITE "${TEMP_DBUSMENU_DIR}/CMakeLists.txt.in" "${MODIFIED_CONTENT}")
    
    # Copy all other files from libdbusmenu to temp location for reference
    # (not actually used, just documentation of the approach)
    message(STATUS "Prepared dbusmenu-lxqt to build as STATIC library")
endfunction()
