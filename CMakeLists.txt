cmake_minimum_required(VERSION 3.14)

project(wordsCounter LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/CPM.cmake)
CPMAddPackage("gh:vimpunk/mio#master")
CPMAddPackage("gh:martinus/unordered_dense@4.11.0")
CPMAddPackage(
    NAME xxHash
    GITHUB_REPOSITORY Cyan4973/xxHash
    VERSION 0.8.3
    SOURCE_SUBDIR cmake_unofficial
    OPTIONS
        "BUILD_SHARED_LIBS OFF"
        "XXHASH_BUILD_XXHSUM OFF"
)

add_executable(${PROJECT_NAME}
    src/AffinityHandler.cpp
    src/SystemUtils.cpp
    src/UniqueWordsCounter.cpp
    src/XXH3Hasher.cpp
    src/main.cpp
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Wall
        -Wextra
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-interference-size>
        $<$<CONFIG:Release>:-march=native -flto=auto>
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${mio_SOURCE_DIR}/include
)

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Threads::Threads
        unordered_dense::unordered_dense
        xxhash
)
