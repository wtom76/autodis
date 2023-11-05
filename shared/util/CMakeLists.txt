cmake_minimum_required(VERSION 3.22.0)

add_library(util OBJECT
	uri.cpp
	field_map.cpp
	file_system.cpp
	logger.cpp
	time.cpp
)

target_include_directories(util PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(util PRIVATE cxx_std_23)

if(MSVC)
	target_compile_options(util PRIVATE /W4 /WX)
else()
	target_compile_options(util PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(util PRIVATE pch.hpp)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(util PUBLIC spdlog::spdlog spdlog::spdlog_header_only)

find_package(nlohmann_json CONFIG REQUIRED)