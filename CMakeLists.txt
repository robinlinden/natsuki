cmake_minimum_required(VERSION 3.14...3.22)

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
    natsuki/natsuki_example.cc
)

target_link_libraries(${PROJECT_NAME}_example
    PRIVATE
        ${PROJECT_NAME}
        Threads::Threads
)

target_compile_features(${PROJECT_NAME}_example
    PRIVATE cxx_std_20
)

# Bench
add_library(${PROJECT_NAME}_benchlib
    bench/bench.cc
)

target_include_directories(${PROJECT_NAME}_benchlib
    PUBLIC .
)

target_link_libraries(${PROJECT_NAME}_benchlib
    PRIVATE Threads::Threads
)

target_compile_features(${PROJECT_NAME}_benchlib
    PUBLIC cxx_std_20
)

add_executable(${PROJECT_NAME}_bench
    bench/main.cc
)

target_link_libraries(${PROJECT_NAME}_bench
    PRIVATE
        ${PROJECT_NAME}
        ${PROJECT_NAME}_benchlib
)

target_compile_features(${PROJECT_NAME}_bench
    PRIVATE cxx_std_20
)
