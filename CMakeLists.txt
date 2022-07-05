cmake_minimum_required(VERSION 3.12)

project(myProject LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)
CPMAddPackage("gh:fmtlib/fmt#8.1.1")
CPMAddPackage("gh:mandreyel/mio#master")

set(SOURCES
    src/main.cpp
)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_include_directories(
    ${CMAKE_PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${mio_SOURCE_DIR}/include
)

find_package(Threads REQUIRED)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE Threads::Threads
    PRIVATE fmt::fmt
)
