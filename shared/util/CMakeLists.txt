cmake_minimum_required(VERSION 3.22.0)

add_library(util OBJECT
	uri.cpp
)

target_include_directories(util PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(util PRIVATE
	cxx_std_23
)

target_precompile_headers(util PRIVATE pch.hpp)
