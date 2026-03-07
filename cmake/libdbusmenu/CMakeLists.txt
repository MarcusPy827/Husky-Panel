# Wrapper for building dbusmenu-lxqt as static library
# This avoids modifying the vendor source code directly

# Temporarily copy and patch libdbusmenu to build as static
set(DBUSMENU_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/3rdparty/libdbusmenu")
set(DBUSMENU_BUILD "${CMAKE_CURRENT_BINARY_DIR}/libdbusmenu")

# Copy the entire libdbusmenu directory to build folder
file(COPY "${DBUSMENU_SOURCE}/" DESTINATION "${DBUSMENU_BUILD}")

# Read and modify the src/CMakeLists.txt
set(DBUSMENU_CMAKE "${DBUSMENU_BUILD}/src/CMakeLists.txt")
file(READ "${DBUSMENU_CMAKE}" DBUSMENU_CONTENT)

# Replace SHARED with STATIC
string(REPLACE "add_library(dbusmenu-lxqt SHARED" "add_library(dbusmenu-lxqt STATIC" DBUSMENU_CONTENT "${DBUSMENU_CONTENT}")

# Write back
file(WRITE "${DBUSMENU_CMAKE}" "${DBUSMENU_CONTENT}")

# Now add the modified subdirectory
add_subdirectory("${DBUSMENU_BUILD}" dbusmenu-build EXCLUDE_FROM_ALL)
