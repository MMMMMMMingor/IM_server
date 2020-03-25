cmake_minimum_required(VERSION 3.10)

project(IM_server)
add_definitions(-std=c++11)

include_directories(./include)
include_directories(./src/message)


set(SOURCE ${SOURCE} src/loguru/loguru.cpp src/message/addressbook.pb.cc)

add_executable(${PROJECT_NAME} src/server/server.cpp ${SOURCE})

target_link_libraries(
        ${PROJECT_NAME}
        pthread
        dl
        protobuf
)

############### 客户端编译 ###############

set(CLIENT "IM_client")
add_executable(${CLIENT} src/client/client.cpp)


############### protobuf 测试 ###############
enable_testing()

add_executable(write_protobuf_test test/write_protobuf_test.cpp src/message/addressbook.pb.cc)
add_test(NAME write_message_test COMMAND write_protobuf_test "addressbook.txt")
SET_TESTS_PROPERTIES(write_message_test
        PROPERTIES PASS_REGULAR_EXPRESSION "finished")

target_link_libraries(
        write_protobuf_test
        protobuf
)

add_executable(read_protobuf_test test/read_protobuf_test.cpp src/message/addressbook.pb.cc)
add_test(NAME read_message_test COMMAND read_protobuf_test "addressbook.txt")
SET_TESTS_PROPERTIES(read_message_test
        PROPERTIES PASS_REGULAR_EXPRESSION "1  mingor  mmmmingor@gmail.com    Mobile phone #: 8888888")

target_link_libraries(
        read_protobuf_test
        protobuf
)