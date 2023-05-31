cmake_minimum_required(VERSION 3.22.0)

add_library(learning OBJECT
	mfn.cpp
)

target_include_directories(learning PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(learning PRIVATE cxx_std_23)

if(MSVC)
	target_compile_options(learning PRIVATE /W4 /WX)
else()
	target_compile_options(learning PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(learning PRIVATE pch.hpp)

#find_package(spdlog CONFIG REQUIRED)
#target_link_libraries(learning PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(learning PRIVATE nlohmann_json::nlohmann_json)
