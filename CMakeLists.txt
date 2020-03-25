cmake_minimum_required(VERSION 3.0)

project(jni.hpp LANGUAGES CXX VERSION 1.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

add_library(
	jni.hpp 
	INTERFACE 
)	

target_include_directories(
	jni.hpp
	INTERFACE
	include
)

add_subdirectory(vendor/Catch2)
add_subdirectory(test)

