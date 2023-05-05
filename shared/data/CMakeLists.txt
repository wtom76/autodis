cmake_minimum_required(VERSION 3.22.0)

add_library(data OBJECT
	frame.cpp
)

target_include_directories(data PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(data PRIVATE cxx_std_23)

if(MSVC)
	target_compile_options(data PRIVATE /W4 /WX)
else()
	target_compile_options(data PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(data PRIVATE pch.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)