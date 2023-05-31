cmake_minimum_required(VERSION 3.22.0)

add_library(keeper STATIC
	config.cpp
	metadata.cpp
	data_write.cpp
	data_read.cpp
)

target_include_directories(keeper PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(keeper PRIVATE cxx_std_23)

if(MSVC)
	target_compile_options(keeper PRIVATE /W4 /WX)
else()
	target_compile_options(keeper PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(keeper PRIVATE pch.hpp)

target_link_directories(keeper PRIVATE
	${CMAKE_BINARY_DIR}/shared
)
target_link_libraries(keeper PRIVATE shared)

find_package(libpqxx CONFIG REQUIRED)
target_link_libraries(keeper PRIVATE libpqxx::pqxx)

#find_package(spdlog CONFIG REQUIRED)
#target_link_libraries(keeper PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

find_package(nlohmann_json CONFIG REQUIRED)
#target_link_libraries(keeper PRIVATE nlohmann_json::nlohmann_json)
