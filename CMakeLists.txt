cmake_minimum_required (VERSION 2.6)
project (Json_Generator C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Sanitizers)

add_library(json_generator
	json_gen.cpp
	sentence.cpp)
add_sanitizers(json_generator)

add_executable(json_gen
	json_util.cpp)

add_executable(test-gen
	test.cpp)

target_link_libraries(test-gen
	json_generator
	gtest
	glog
	gflags
	jansson)

add_sanitizers(test-gen)
