cmake_minimum_required(VERSION 2.8)
project( homework )
find_package(PkgConfig REQUIRED)

file(GLOB_RECURSE homework_src
    "src/*.cpp"
    "src/*/*.cpp"
)

add_executable(homework  ${homework_src})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
target_link_libraries(homework)
