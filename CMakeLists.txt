cmake_minimum_required(VERSION 3.1.0)
add_definitions(-std=c++11)
add_executable(kmeantest kmeanmain.cpp)
target_link_libraries(kmeantest ${Boost_LIBRARIES})