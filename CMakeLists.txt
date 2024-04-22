cmake_minimum_required(VERSION 3.10)

project(socket_server)

file(GLOB_RECURSE SRC 
    src/*.cc
    src/CwHttp/*.cc
    src/CwNetWork/*.cc
    src/CwUtil/*.cc
    )

set(CMAKE_CXX_STANDARD 14)


include_directories(include/cppwinks)

message("cmake_source_dir = ${CMAKE_SOURCE_DIR}")



add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} pthread
    #cwutil cwhttp cwnetwork
    #${CMAKE_SOURCE_DIR}/lib/libcwutil.so
    #${CMAKE_SOURCE_DIR}/lib/libcwhttp.so
    #${CMAKE_SOURCE_DIR}/lib/libcwnetwork.so
    )
