cmake_minimum_required(VERSION 3.17.0)
project(path_tracer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(IMAGE_WIDTH 800 CACHE STRING "Image width")
set(IMAGE_HEIGHT 800 CACHE STRING "Image height")
set(RAY_MAX_DEPTH 800 CACHE STRING "Maximum reflection/refraction count")
set(RAYS_PER_PIXEL 800 CACHE STRING "Rays per pixel")
set(FOV 90. CACHE STRING "Field of view")

configure_file(cmake/configuration.hpp.in configuration.hpp)
include_directories(${PROJECT_BINARY_DIR})

enable_testing()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(path_tracer "")
add_subdirectory(src)
target_link_libraries(path_tracer png)

set_property(TARGET path_tracer PROPERTY INTERPROCEDURAL_OPTIMIZATION True)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
