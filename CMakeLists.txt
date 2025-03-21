cmake_minimum_required(VERSION 3.31)

project(pipette LANGUAGES CXX)

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

add_executable(pipette-server)
target_sources(pipette-server PRIVATE "app/server.cpp" 
    "pipette/server/server.cpp"
    "pipette/server/server.hpp"
    "pipette/server/event_loop.cpp"
    "pipette/server/event_loop.hpp"
)
target_link_libraries(pipette-server PRIVATE Sockpp::sockpp-static spdlog::spdlog)
target_include_directories(pipette-server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


add_executable(pipette-client)
target_sources(pipette-client PRIVATE "app/client.cpp" 
    "pipette/client/client.cpp"
    "pipette/client/client.hpp"
)
target_link_libraries(pipette-client PRIVATE Sockpp::sockpp-static spdlog::spdlog)
target_include_directories(pipette-client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
