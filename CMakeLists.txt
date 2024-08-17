cmake_minimum_required(VERSION 3.12)
project(WsOrder)

set(CMAKE_CXX_STANDARD 17)

# Boost
find_package(Boost REQUIRED)

# Protobuf
include(FindProtobuf)
find_package(Protobuf REQUIRED)

# OpenSSL
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

file(GLOB ProtoSrc ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.cc)

add_library(WsOrder
        src/ws_connection.cpp
        src/ws_connection.cpp
        src/main_app.cpp
        src/sender.cpp
        src/receiver.cpp
        src/ws_client.cpp
        src/raw_message_parser.cpp
        src/exchange_client.cpp
        ${ProtoSrc}
)


target_include_directories(
            WsOrder
        PUBLIC
            ${Boost_INCLUDE_DIRS}
            ${PROTOBUF_INCLUDE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/proto
            ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
            WsOrder
        PUBLIC
            ${PROTOBUF_LIBRARY}
            OpenSSL::SSL
            OpenSSL::Crypto
)


add_executable(example examples/main.cpp)
target_link_libraries(example PRIVATE WsOrder)