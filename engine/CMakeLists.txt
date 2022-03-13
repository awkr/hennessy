project(Engine VERSION 0.1.0 LANGUAGES C CXX)

find_package(Vulkan REQUIRED FATAL_ERROR)

set(HEADERS
    src/defines.h
    src/core/log.h
    src/platform/platform.h
    )

set(SOURCES
    src/platform/platform_macos.mm)

set(Engine_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(Engine_INCLUDE_DIR ${Engine_INCLUDE_DIR} PARENT_SCOPE)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Headers)
if (APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED FATAL_ERROR)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_LIBRARY}
        "-framework QuartzCore")
endif ()
target_include_directories(${PROJECT_NAME} PRIVATE
    ${Engine_INCLUDE_DIR}
    ${Vulkan_INCLUDE_DIRS})

