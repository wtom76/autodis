cmake_minimum_required(VERSION 3.22.0)

project(seriescfg VERSION 0.1.0)

add_executable(seriescfg
	main.cpp
	config.cpp
	series_params.cpp
	op_create.cpp
	op_delete.cpp
)

target_include_directories(seriescfg PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(seriescfg PRIVATE cxx_std_23)

if(MSVC)
	target_compile_options(seriescfg PRIVATE /W4 /WX)
else()
	target_compile_options(seriescfg PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(seriescfg
	PRIVATE pch.hpp
)

target_link_directories(seriescfg PRIVATE
	${CMAKE_BINARY_DIR}/shared
)
target_link_libraries(seriescfg PRIVATE shared)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(seriescfg PRIVATE spdlog::spdlog)

find_package(libpqxx CONFIG REQUIRED)
target_link_libraries(seriescfg PRIVATE libpqxx::pqxx)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(seriescfg PRIVATE nlohmann_json::nlohmann_json)
