cmake_minimum_required(VERSION 3.30)

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
set(BOOST_INCLUDE_LIBRARIES unordered)
CPMAddPackage(
  NAME Boost
  GITHUB_REPOSITORY boostorg/boost
  GIT_TAG boost-1.92.0
)
CPMAddPackage(
  NAME absl
  GITHUB_REPOSITORY abseil/abseil-cpp
  GIT_TAG 20260817.0
  OPTIONS
    "ABSL_PROPAGATE_CXX_SETTINGS ON"
    "ABSL_ENABLE_INSTALL ON"
    "BUILD_TESTING OFF"
)
CPMAddPackage(
  NAME wyhash
  GITHUB_REPOSITORY wangyi-fudan/wyhash
  GIT_TAG wyhash_final4
)
CPMAddPackage(
  NAME mimalloc
  GITHUB_REPOSITORY microsoft/mimalloc
  GIT_TAG v3.5.3
  OPTIONS
    "MI_BUILD_SHARED OFF" # Build static library
    "MI_BUILD_OBJECT OFF" # Do not build separate object file
    "MI_BUILD_TESTS OFF"  # Disable unit tests
    "MI_OVERRIDE ON"      # Automatically override C/C++ malloc/free/new/delete
    "MI_GUARDED OFF"      # Disables guarded sampling allocator
)

add_executable(${PROJECT_NAME}
  src/main.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    -Wall
    -Wextra
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-interference-size>
    $<$<CONFIG:Release>:-march=native>
)

# Workaround for GCC15: Locate libstdc++'s std.cc module file dynamically
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    string(REGEX MATCH "^[0-9]+" GCC_MAJOR "${CMAKE_CXX_COMPILER_VERSION}")

    foreach(PREFIX /usr /usr/local)
        if(EXISTS "${PREFIX}/include/c++/${GCC_MAJOR}/bits/std.cc")
            set(GCC_STD_MODULE "${PREFIX}/include/c++/${GCC_MAJOR}/bits/std.cc")
            set(GCC_STD_DIR "${PREFIX}/include/c++/${GCC_MAJOR}/bits")
            break()
        endif()
    endforeach()

    if(NOT GCC_STD_MODULE)
        message(FATAL_ERROR "Could not find std.cc for GCC ${GCC_MAJOR}.")
    endif()
endif()

target_sources(${PROJECT_NAME}
  PRIVATE
    FILE_SET std_module TYPE CXX_MODULES BASE_DIRS "${GCC_STD_DIR}" FILES ${GCC_STD_MODULE}
    FILE_SET CXX_MODULES TYPE CXX_MODULES FILES
      src/AffinityHandler.cppm
      src/Simd.cppm
      src/SystemUtils.cppm
      src/UniqueWordsCounter.cppm
      src/XXH3Hasher.cppm
)

find_package(Threads REQUIRED)

#target_include_directories(${PROJECT_NAME} PRIVATE "${absl_SOURCE_DIR}")

if(wyhash_ADDED AND NOT TARGET wyhash::wyhash)
  add_library(wyhash::wyhash INTERFACE IMPORTED GLOBAL)
  target_include_directories(wyhash::wyhash INTERFACE "${wyhash_SOURCE_DIR}")
endif()

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Threads::Threads
    mio
    unordered_dense::unordered_dense
    xxhash
    Boost::unordered
    absl::flat_hash_set
    wyhash::wyhash
    mimalloc-static
)
