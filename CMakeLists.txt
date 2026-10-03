cmake_minimum_required(VERSION 4.2)

project(wordsCounter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_LINKER_TYPE MOLD)

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
    FILE_SET std_module TYPE CXX_MODULES
      BASE_DIRS "${GCC_STD_DIR}"
      FILES
        ${GCC_STD_MODULE}
    FILE_SET CXX_MODULES TYPE CXX_MODULES
      FILES
        src/AffinityHandler.cppm
        src/Simd.cppm
        src/SystemUtils.cppm
        src/UniqueWordsCounter.cppm
        src/Hasher.cppm
  PRIVATE
    src/Hasher.cpp
)

find_package(Threads REQUIRED)
find_package(mimalloc)
find_package(jemalloc REQUIRED)
find_package(mio)
find_package(Boost)
find_package(folly)
find_package(robin_hood)
find_package(unordered_dense)
find_package(tsl-robin-map)
find_package(tsl-sparse-map)
find_package(xxHash)
find_package(absl)
find_package(wyhash)
find_package(rapidhash)


target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Threads::Threads
    #mimalloc-static
    jemalloc::jemalloc
    mio::mio
    boost::boost
    folly::folly
    robin_hood::robin_hood
    unordered_dense::unordered_dense
    tsl::robin_map
    tsl::sparse_map
    xxHash::xxhash
    abseil::abseil
    wyhash::wyhash
    rapidhash::rapidhash
)
