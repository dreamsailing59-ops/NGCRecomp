cmake_minimum_required(VERSION 3.12)
project(NGCRecomp C)

add_subdirectory(external/capstone EXCLUDE_FROM_ALL)

add_executable(NGCRecomp src/boot.c)

target_link_libraries(NGCRecomp PRIVATE capstone)

target_include_directories(NGCRecomp PRIVATE external/capstone/include)