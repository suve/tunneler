cmake_minimum_required(VERSION 3.19)

# name + version from configure.ac's AC_INIT
project(tunneler LANGUAGES C VERSION 1.1.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wfloat-equal -Wparentheses")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=incompatible-pointer-types -Werror=discarded-qualifiers -Werror=int-conversion -Werror=div-by-zero -Werror=sequence-point -Werror=uninitialized -Werror=duplicated-cond -Werror=return-mismatch -Werror=return-type")

find_package(SDL2 REQUIRED)

# values from autotools-generated config.h
set(PACKAGE "${CMAKE_PROJECT_NAME}")
set(PACKAGE_BUGREPORT "tvkalvas@jyu.fi")
set(PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set(PACKAGE_STRING "${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_VERSION}")
set(PACKAGE_TARNAME "${CMAKE_PROJECT_NAME}")
set(PACKAGE_URL "")
set(PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set(VERSION "${CMAKE_PROJECT_VERSION}")
configure_file(src/config.h.cmake-in src/config.h)

add_executable(tunneler
               src/ai.c
               src/config-file.c
               src/graphics.c
               src/keys.c
               src/main.c
               src/terrain.c
               src/timer.c
               src/tunneler.c)

add_custom_target(fmt
                  COMMAND clang-format "--style=file:${PROJECT_SOURCE_DIR}/.clang-format" -i "${PROJECT_SOURCE_DIR}/src/*.c" "${PROJECT_SOURCE_DIR}/src/*.h")

# for config.h
target_include_directories(tunneler PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/src")
target_link_libraries(tunneler PRIVATE SDL2::SDL2 m)
