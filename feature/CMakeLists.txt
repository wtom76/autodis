cmake_minimum_required(VERSION 3.22.0)

add_library(feature OBJECT
	impl/stored.cpp
	impl/sma.cpp
	impl/delta.cpp
	shop.cpp
	master_index.cpp
)

target_include_directories(feature PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(feature PRIVATE cxx_std_23)

if(MSVC)
	target_compile_options(feature PRIVATE /W4 /WX)
else()
	target_compile_options(feature PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(feature PRIVATE pch.hpp)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(feature PRIVATE spdlog::spdlog)
