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
      src/SystemUtils.cppm
      src/UniqueWordsCounter.cppm
      src/XXH3Hasher.cppm
)

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Threads::Threads
    mio
    unordered_dense::unordered_dense
    xxhash
)
