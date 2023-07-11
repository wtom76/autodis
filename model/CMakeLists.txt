cmake_minimum_required(VERSION 3.22.0)

add_library(model OBJECT
	model_000.cpp
	model_001.cpp
	model_002.cpp
)

target_include_directories(model PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(model PRIVATE cxx_std_23)

if(MSVC)
	target_compile_options(model PRIVATE /W4 /WX)
else()
	target_compile_options(model PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(model PRIVATE pch.hpp)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(model PRIVATE nlohmann_json::nlohmann_json)