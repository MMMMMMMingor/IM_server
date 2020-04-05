cmake_minimum_required(VERSION 3.10)

project(IM_server)
add_compile_options(-std=c++11)
add_compile_options(-Wall)
#add_compile_options(-O3)

include_directories(./include)
include_directories(./src/message)


set(SOURCE ${SOURCE} src/loguru/loguru.cpp src/yaml/yaml.cpp src/message/message.pb.cc)

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

add_executable(test_add test/test_add.cpp)
add_test(NAME test_add COMMAND test_add 1 2)
SET_TESTS_PROPERTIES(test_add
        PROPERTIES PASS_REGULAR_EXPRESSION "3")


add_executable(test_yaml test/test_yaml.cpp src/yaml/yaml.cpp)
add_test(NAME test_yaml COMMAND test_yaml)
SET_TESTS_PROPERTIES(test_yaml
        PROPERTIES PASS_REGULAR_EXPRESSION "9999")

add_executable(test_snowflake test/test_snowflake.cpp)

add_executable(test_session test/test_session.cpp)

add_executable(test_keepalive test/test_keepalive.cpp)

add_executable(test_history test/test_history.cpp src/message/message.pb.cc)
target_link_libraries(
        test_history
        protobuf
)

add_executable(test_protobuf test/test_protobuf.cpp src/message/message.pb.cc src/loguru/loguru.cpp)
target_link_libraries(
        test_protobuf
        protobuf
        dl
        pthread
)