project(Test LANGUAGES C CXX)

file(GLOB_RECURSE SOURCES *.cc)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Engine)
target_include_directories(${PROJECT_NAME} PRIVATE ${Engine_INCLUDE_DIR})
