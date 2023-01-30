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
		"../"
)

target_compile_features(collector
    PUBLIC
        cxx_std_23
)

target_precompile_headers(collector
	PUBLIC pch.h
	PUBLIC feed/pch.h
	PUBLIC source/pch.h
)

target_link_libraries(collector PRIVATE ${CMAKE_BINARY_DIR}/shared/libshared.a)
target_link_libraries(collector PRIVATE ${CMAKE_BINARY_DIR}/keeper/libkeeper.a)

find_package(libpqxx CONFIG REQUIRED)
target_link_libraries(collector PRIVATE libpqxx::pqxx)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(collector PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(collector PRIVATE nlohmann_json::nlohmann_json)
