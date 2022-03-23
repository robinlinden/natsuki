cmake_minimum_required(VERSION 3.18)

project(natsuki)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# Library
find_package(Asio REQUIRED)

add_library(${PROJECT_NAME}
    natsuki/natsuki.cc
    natsuki/natsuki.h
)

target_include_directories(${PROJECT_NAME}
    PUBLIC .
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC Asio::Asio
)

target_compile_features(${PROJECT_NAME}
    PUBLIC cxx_std_20
)

# Example
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}_example
    example/natsuki.cc
)

target_link_libraries(${PROJECT_NAME}_example
    PRIVATE
        ${PROJECT_NAME}
        Threads::Threads
)

target_compile_features(${PROJECT_NAME}_example
    PRIVATE cxx_std_20
)
