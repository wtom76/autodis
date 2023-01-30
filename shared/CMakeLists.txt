cmake_minimum_required(VERSION 3.22.0)

project(shared VERSION 0.1.0)

add_library(shared STATIC
	util/uri.cpp
)

target_include_directories(shared PUBLIC
	"./"
)

target_compile_features(shared PUBLIC
	cxx_std_23
)

target_precompile_headers(shared
	PUBLIC pch.h
	PUBLIC util/pch.h
)
