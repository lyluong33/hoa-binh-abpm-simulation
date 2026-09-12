cmake_minimum_required(VERSION 3.20)

file(GLOB cppfiles CONFIGURE_DEPENDS *.cpp)
add_executable(agp24 ${cppfiles})
target_include_directories(agp24 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# GLPK >= 4.52 (glp_* API). Point GLPK_LIBRARY_DIR at a directory containing
# libglpk.so when it is not installed system-wide.
set(GLPK_LIBRARY_DIR "${PROJECT_SOURCE_DIR}/libs/glpk4.45" CACHE PATH "Directory holding libglpk")
find_library(GLPK_LIBRARY NAMES glpk glpk_4_45_x64 HINTS "${GLPK_LIBRARY_DIR}" REQUIRED)
target_link_libraries(agp24 PRIVATE ${GLPK_LIBRARY})

if(NOT MSVC)
  target_compile_options(agp24 PRIVATE -w)
  get_filename_component(GLPK_RPATH "${GLPK_LIBRARY}" DIRECTORY)
  set_target_properties(agp24 PROPERTIES BUILD_RPATH "${GLPK_RPATH}")
  target_link_options(agp24 PRIVATE -Wl,--disable-new-dtags)
endif()
