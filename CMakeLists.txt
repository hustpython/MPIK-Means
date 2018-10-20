cmake_minimum_required(VERSION 3.1.0)
add_definitions(-std=c++11)
add_executable(datasetreader datareadertest.cpp)
target_link_libraries(datasetreader ${Boost_LIBRARIES})