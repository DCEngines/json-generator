cmake_minimum_required (VERSION 2.6)
project (Json_Generator C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# use flag -DSANITIZE_ADDRESS=on to build with address sanitizer 
find_package(Sanitizers)

set(COMMON_LIBS
	json_generator
	gtest
	glog
	gflags
	jansson)

add_library(json_generator
	json_gen.cpp
	sentence.cpp)
add_sanitizers(json_generator)

add_executable(json_gen
	json_util.cpp)

target_link_libraries(json_gen
	${COMMON_LIBS})

add_sanitizers(json_gen)

add_executable(test-gen
	test.cpp)

target_link_libraries(test-gen
	${COMMON_LIBS})

add_sanitizers(test-gen)
