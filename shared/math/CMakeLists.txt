cmake_minimum_required(VERSION 3.22.0)

add_library(math OBJECT
	track.cpp
	sma_delta.cpp
)

target_include_directories(math PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(math PRIVATE cxx_std_23)

if(MSVC)
	target_compile_options(math PRIVATE /W4 /WX)
else()
	target_compile_options(math PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(math PRIVATE pch.hpp)
