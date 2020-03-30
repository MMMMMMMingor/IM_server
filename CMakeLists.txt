cmake_minimum_required(VERSION 3.10)

project(IM_server)
add_definitions(-std=c++11)

include_directories(./include)
include_directories(./src/message)


set(SOURCE ${SOURCE} src/loguru/loguru.cpp src/message/message.pb.cc)

add_executable(${PROJECT_NAME} src/server/server.cpp ${SOURCE})

target_link_libraries(
        ${PROJECT_NAME}
        pthread
        dl
        protobuf
)

############### 客户端编译 ###############

set(CLIENT "IM_client")
add_executable(${CLIENT} src/client/client.cpp ${SOURCE})

target_link_libraries(
        ${CLIENT}
        pthread
        dl
        protobuf
)


############### 测试 ###############
enable_testing()

add_executable(test_add test/test.cpp)
add_test(NAME test COMMAND test_add 1 2)
SET_TESTS_PROPERTIES(test
        PROPERTIES PASS_REGULAR_EXPRESSION "3")

