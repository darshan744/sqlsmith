cmake_minimum_required(VERSION 3.14)
project(sqlsmith VERSION 1.3)

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
add_executable(${PROJECT_NAME} ${SQLSmith_SRC})
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
target_compile_definitions(${PROJECT_NAME}
PRIVATE SQLSMITH_VERSION="${PROJECT_VERSION}")

if(NOT CMAKE_TOOLCHAIN_FILE)
    message(
        FATAL
        "Please install libpqxx and provide the vcpkg.cmae in CMAKE_TOOLCHAIN_FILE"
    )
endif()

find_package(libpqxx CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE libpqxx:pqxx)
