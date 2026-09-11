cmake_minimum_required(VERSION 4.4.3)

project(backtesting LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -march=native)

add_executable(${PROJECT_NAME} ./src/main.cpp)

add_subdirectory(parse_csv)
