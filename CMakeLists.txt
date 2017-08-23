cmake_minimum_required(VERSION 3.1)

project(bitmask)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wpedantic -Wall")
endif()

enable_testing()

include(GNUInstallDirs)

add_library(bitmask INTERFACE)
target_sources(bitmask INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/bitmask/bitmask.hpp>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/bitmask/bitmask.hpp>
)

target_include_directories(bitmask INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS bitmask EXPORT bitmaskConfig)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT bitmaskConfig DESTINATION share/bitmask/cmake)
export(TARGETS bitmask FILE bitmaskConfig.cmake)

add_subdirectory(test)
