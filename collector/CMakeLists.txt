cmake_minimum_required(VERSION 3.22.0)

project(collector VERSION 0.1.0)

add_executable(collector
	factory.cpp
	main.cpp
)

target_include_directories(collector PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(collector
	PRIVATE
        cxx_std_23
)

target_precompile_headers(collector
	PRIVATE pch.hpp
	PRIVATE feed/pch.hpp
	PRIVATE source/pch.hpp
)

add_subdirectory(feed)
add_subdirectory(source)
target_link_libraries(collector PRIVATE feed)
target_link_libraries(collector PRIVATE source)

target_link_directories(collector PRIVATE
	${CMAKE_BINARY_DIR}/shared
	${CMAKE_BINARY_DIR}/keeper
)
target_link_libraries(collector PRIVATE shared)
target_link_libraries(collector PRIVATE keeper)

find_package(libpqxx CONFIG REQUIRED)
target_link_libraries(collector PRIVATE libpqxx::pqxx)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(collector PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(collector PRIVATE nlohmann_json::nlohmann_json)
