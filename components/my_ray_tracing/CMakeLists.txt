cmake_minimum_required(VERSION 3.18)

# 设置名称， 会在"/components”文件夹下生成  名称.dll
set(MY_COMPONENT_NAME "RayTracer")

# Use CONFIGURE_DEPENDS so CMake regenerates when files are added/renamed.
file(GLOB_RECURSE COMP_HEADER_FILES CONFIGURE_DEPENDS "./include/*.h" "./include/*.hpp")
source_group("Header Files" FILES ${COMP_HEADER_FILES})

# On Windows the filesystem is often case-insensitive, but on others it is not.
# Make sure we pick up both .cpp and .CPP consistently.
file(GLOB_RECURSE COMP_SOURCE_FILES CONFIGURE_DEPENDS "./src/*.cpp" "./src/*.CPP")

add_library(${MY_COMPONENT_NAME} SHARED "${COMP_SOURCE_FILES}" "${COMP_HEADER_FILES}")
target_link_libraries(${MY_COMPONENT_NAME} NRServer)

include_directories("./include")
