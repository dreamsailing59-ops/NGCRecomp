cmake_minimum_required(VERSION 3.12)
project(NGCRecomp C)

add_executable(NGCRecomp src/boot.c)

target_link_libraries(NGCRecomp PRIVATE "${CMAKE_SOURCE_DIR}/external/capstone/build/libcapstone.a")

target_include_directories(NGCRecomp PRIVATE "${CMAKE_SOURCE_DIR}/external/capstone/include")