cmake_minimum_required(VERSION 3.22.0)

project(collector VERSION 0.1.0)

add_executable(collector
	source/file.cpp
	feed/finam_daily_file.cpp
	main.cpp
)

target_include_directories(collector
	PUBLIC
		"./"
)

target_compile_features(collector
    PUBLIC
        cxx_std_23
)

target_precompile_headers(collector
	PUBLIC
		pch.h
)