set(SRC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(path_tracer
PRIVATE
${SRC}/main.cpp
${SRC}/save_png.hpp
${SRC}/Material.hpp
${SRC}/Ray.hpp
${SRC}/Sphere.hpp
${SRC}/Timer.hpp)