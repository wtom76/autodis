cmake_minimum_required(VERSION 3.22.0)

project(shared VERSION 0.1.0)

add_subdirectory(util)

add_library(shared STATIC
	shared.cpp
)

target_link_libraries(shared PRIVATE util)

target_include_directories(shared PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(shared PRIVATE cxx_std_23)

if(MSVC)
	target_compile_options(shared PRIVATE /W4 /WX)
else()
	target_compile_options(shared PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(shared
	PRIVATE pch.hpp
	PRIVATE util/pch.hpp
	PRIVATE data/pch.hpp
)
