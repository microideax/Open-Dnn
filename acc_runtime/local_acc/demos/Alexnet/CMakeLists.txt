cmake_minimum_required(VERSION 2.8.8)

find_package(Caffe)
include_directories(${Caffe_INCLUDE_DIRS})
add_definitions(${Caffe_DEFINITIONS})    # ex. -DCPU_ONLY

find_package( OpenCV REQUIRED )

include_directories(../../api_lib)
link_directories(../../api_lib)

set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs")

add_executable(read_proto RprotoW.cpp)
target_link_libraries(read_proto ${Caffe_LIBRARIES} api_lib ${OpenCV_LIBS})
