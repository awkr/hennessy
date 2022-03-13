cmake_minimum_required(VERSION 3.20)
project(hennessy LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(engine)
add_subdirectory(test)
