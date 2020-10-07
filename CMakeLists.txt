cmake_minimum_required(VERSION 3.8)
project(annotation_graph)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Only list install files that have actually changed.
set(CMAKE_INSTALL_MESSAGE "LAZY")

IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -std=libc++17")
ENDIF()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_MACOSX_RPATH 1)
endif()
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(BOOST_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.46 COMPONENTS system filesystem)

IF(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
ELSE(Boost_FOUND)
    message(FATAL_ERROR "Boost libraries not found. Please install them")
ENDIF(Boost_FOUND)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(src)