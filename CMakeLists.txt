cmake_minimum_required(VERSION 3.21)
project(tyr VERSION 0.0.0)


##############################################################
# Language setup
##############################################################

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


##############################################################
# Options
##############################################################


##############################################################
# Build Targets
##############################################################

option(BUILD_PYTYR "Build" OFF)
option(BUILD_TESTS "Build" OFF)
option(BUILD_EXECUTABLES "Build" OFF)
option(BUILD_PROFILING "Build" OFF)

##############################################################
# Common Settings
##############################################################

# make cache variables for install destinations
include(GNUInstallDirs)

if(MSVC)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W1 /EHsc /bigobj /MP")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " /IGNORE:4006,4044,4075")
else()
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    endif()
    # TODO: Add -Wextra and fix all warnings

    message("CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")

    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    add_compile_options(-Wall)

    # Use lld if present (won't error if missing)
    find_program(LLD_LINKER NAMES ld.lld lld)
    if(LLD_LINKER)
        add_link_options(-fuse-ld=lld)
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

        # Release: enable LTO but cap parallelism to reduce RAM spikes
        set(CMAKE_CXX_FLAGS_RELEASE
            "${CMAKE_CXX_FLAGS_RELEASE} -flto=4")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE
            "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto=4")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE
            "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -flto=4")

        # RelWithDebInfo: make debug info cheap (big compile-time/RAM win)
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
            "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g2 -fno-omit-frame-pointer -fno-lto")

        # Debug: keep your choices (CMake will supply -O0 or -Og depending on toolchain,
        # but if you want -Og specifically, keep it here; otherwise omit -Og)
        set(CMAKE_CXX_FLAGS_DEBUG
            "${CMAKE_CXX_FLAGS_DEBUG} -Og -g2 -ggdb")

    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")

        # Release: prefer ThinLTO for much lower compile/link cost than full LTO
        set(CMAKE_CXX_FLAGS_RELEASE
            "${CMAKE_CXX_FLAGS_RELEASE} -flto=thin")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE
            "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--thinlto-jobs=4")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE
            "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -Wl,--thinlto-jobs=4")

        # RelWithDebInfo: cheap debug info
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
            "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g2 -fno-omit-frame-pointer -fno-lto")

        # Debug (same note as above)
        set(CMAKE_CXX_FLAGS_DEBUG
            "${CMAKE_CXX_FLAGS_DEBUG} -Og -g2 -ggdb")

    endif()
endif()

message(STATUS "Build configuration: ${CMAKE_BUILD_TYPE}")

set(DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data/")
add_definitions(-DDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")
message("DATA_DIR: ${DATA_DIR}")

if(VERBOSE)
    add_compile_definitions(VERBOSE)
endif()

##############################################################
# CMake modules and macro files
##############################################################

list(APPEND CMAKE_MODULE_PATH
  "${PROJECT_SOURCE_DIR}/cmake"
)
include("configure_boost")
include("configure_ccache")

##############################################################
# CCache
##############################################################

# CCache
configure_ccache()


##############################################################
# Dependency Handling
##############################################################

# set(CMAKE_FIND_DEBUG_MODE TRUE)


find_package(absl CONFIG REQUIRED PATHS ${CMAKE_PREFIX_PATH} NO_DEFAULT_PATH)
if(absl_FOUND)
  message(STATUS "Found absl: ${absl_DIR} (found version ${absl_VERSION})")
endif()


find_package(argparse CONFIG REQUIRED PATHS ${CMAKE_PREFIX_PATH} NO_DEFAULT_PATH)
if(argparse_FOUND)
  message(STATUS "Found argparse: ${argparse_DIR} (found version ${argparse_VERSION})")
endif()


# Boost
# Find Boost headers only according to https://cmake.org/cmake/help/latest/module/FindBoost.html
configure_boost()
find_package(Boost ${BOOST_MIN_VERSION} REQUIRED COMPONENTS iostreams PATHS ${CMAKE_PREFIX_PATH} NO_DEFAULT_PATH)
if(Boost_FOUND)
  message(STATUS "Found Boost: ${Boost_DIR} (found version ${Boost_VERSION})")
endif()


find_package(cista CONFIG REQUIRED PATHS ${CMAKE_PREFIX_PATH} NO_DEFAULT_PATH)
if(cista_FOUND)
  message(STATUS "Found cista: ${cista_DIR} (found version ${cista_VERSION})")
endif()


find_package(fmt REQUIRED PATHS ${CMAKE_PREFIX_PATH} NO_DEFAULT_PATH)
if(fmt_FOUND)
  message(STATUS "Found fmt: ${fmt_DIR} (found version ${fmt_VERSION})")
endif()


# Prefer -pthread over -lpthread on platforms that support it
set(THREADS_PREFER_PTHREAD_FLAG ON)
# CMake's built-in module; no PATHS/NO_DEFAULT_PATH here
find_package(Threads REQUIRED)
if(Threads_FOUND)
  message(STATUS "Found Threads: ${CMAKE_THREAD_LIBS_INIT}")
endif()


find_package(TBB CONFIG REQUIRED PATHS ${CMAKE_PREFIX_PATH} NO_DEFAULT_PATH)
if(TBB_FOUND)
  include_directories(${TBB_INCLUDE_DIRS})
  message(STATUS "Found TBB: ${TBB_DIR} (found version ${TBB_VERSION})")
endif()


find_package(valla CONFIG COMPONENTS core REQUIRED PATHS ${CMAKE_PREFIX_PATH} NO_DEFAULT_PATH)
if(valla_FOUND)
  message(STATUS "Found valla: ${valla_DIR} (found version ${valla_VERSION})")
endif()


find_package(loki ${LOKI_MIN_VERSION} COMPONENTS parsers REQUIRED PATHS ${CMAKE_PREFIX_PATH} NO_DEFAULT_PATH)
if(loki_FOUND)
  message(STATUS "Found loki: ${loki_DIR} (found version ${loki_VERSION})")
endif()


##############################################################
# Add library and executable targets
##############################################################

# ------------
# Target Tyr
# ------------
add_subdirectory(src)

# -------------------
# Target Python Tyr
# -------------------
if(BUILD_PYTYR)
    add_subdirectory(python/src/pytyr)
endif()

# ----------
# Target Exe
# ----------
if (BUILD_EXECUTABLES)
    add_subdirectory(exe)
endif()

# ----------------
# Target Profiling
# ----------------
if(BUILD_PROFILING)
    add_subdirectory(benchmark)
endif()

# -----------
# Target Test
# -----------
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()



###########
# Install #
###########

# Install header files
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/tyr"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# Install cmake scripts
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cmake/"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tyr/cmake"
)


###########
# Exports #
###########

# https://cmake.org/cmake/help/latest/guide/importing-exporting/index.html

include(CMakePackageConfigHelpers)

# Generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/tyrConfigVersion.cmake"
    VERSION ${tyr_VERSION}
    COMPATIBILITY ExactVersion
)

# Create config file
# https://cmake.org/cmake/help/book/mastering-cmake/cmake/Help/guide/importing-exporting/index.html
# https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html#generating-a-package-configuration-file
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/tyrConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tyr"
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# Install config files
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/tyrConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/tyrConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tyr"
)
