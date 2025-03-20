cmake_minimum_required(VERSION 3.31)

project(pippette LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

find_program(SCCACHE sccache REQUIRED)

set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE})
set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE})
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded)
cmake_policy(SET CMP0141 NEW)

find_package(sockpp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(pippette-server "app/server.cpp")
target_link_libraries(pippette-server PRIVATE Sockpp::sockpp-static spdlog::spdlog)


add_executable(pippette-client "app/client.cpp")
target_link_libraries(pippette-client PRIVATE Sockpp::sockpp-static spdlog::spdlog)