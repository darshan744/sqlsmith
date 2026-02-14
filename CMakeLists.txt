cmake_minimum_required(VERSION 3.14)
project(SQLSmith VERSION 1.4.0 DESCRIPTION "SQL Fuzzer" LANGUAGES CXX)

find_program(CCACHE_PROGRAM  ccache)

if(CCACHE_PROGRAM)
    message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
else()
    message(STATUS "ccache not found, proceeding without it.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(sqlsmith)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR 
        "\n❌ vcpkg toolchain file not specified!\n"
        "\nTo build with vcpkg, run:\n"
        "  cmake -B build -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake\n"
        "\nExample:\n"
        "  cmake -B build -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake\n"
    )
endif()

target_include_directories(sqlsmith PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # Our header files (.hh)
    ${CMAKE_CURRENT_BINARY_DIR}          # Generated files (config.h, gitrev.h)
)
file(GLOB SOURCES "src/*.cc")
target_sources(sqlsmith PRIVATE ${SOURCES})

find_package(libpqxx CONFIG REQUIRED)
target_link_libraries(sqlsmith PRIVATE libpqxx::pqxx)

# Generate gitrev.h with git version
set(GIT_TAG "UNRELEASED")
execute_process(
    COMMAND git describe --exclude=debian* --dirty --tags --always
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/gitrev.h "#define GITREV \"${GIT_TAG}\"\n")

