cmake_minimum_required (VERSION 2.6)
project (Json_Generator C CXX)
#set (CXX_STANDARD "c++14")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(json_generator
	json_gen.cpp
	sentence.cpp)
	

add_executable(json_gen
	json_util.cpp)

add_executable(test
	test.cpp)

target_link_libraries(test
	json_generator
	gtest
	glog
	gflags
	jansson)
