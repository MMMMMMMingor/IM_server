cmake_minimum_required(VERSION 3.10)

project(IM_server)
add_definitions(-std=c++11)

include_directories(./include)

aux_source_directory(${PROJECT_NAME} ./src/loguru)

set(SOURCE ${SOURCE} ./src/loguru/loguru.cpp src/server/dispatcher.hpp src/server/handler.hpp)

add_executable(${PROJECT_NAME} src/server/server.cpp ${SOURCE})

target_link_libraries(
        ${PROJECT_NAME}
        pthread
        dl
)

set(CLIENT "IM_client")
add_executable(${CLIENT} src/client/client.cpp)
