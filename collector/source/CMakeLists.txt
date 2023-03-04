cmake_minimum_required(VERSION 3.22.0)

add_library(source OBJECT
	file.cpp
)

target_include_directories(source PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(source PRIVATE
	cxx_std_23
)

if(MSVC)
	target_compile_options(source PRIVATE /W4 /WX)
else()
	target_compile_options(source PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(source PRIVATE pch.hpp)

find_package(libpqxx CONFIG REQUIRED)
target_link_libraries(source PRIVATE libpqxx::pqxx)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)