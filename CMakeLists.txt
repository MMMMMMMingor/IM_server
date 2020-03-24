cmake_minimum_required(VERSION 3.10)

project(IM_server)
add_definitions(-std=c++11)

set(CLIENT "client")

include_directories(./include)

aux_source_directory(${PROJECT_NAME} ./src/loguru)

add_executable(${PROJECT_NAME} src/server/server.cpp src/loguru/loguru.cpp)
add_executable(${CLIENT} src/client/client.cpp )

target_link_libraries(
    ${PROJECT_NAME}
    pthread
    dl
)

