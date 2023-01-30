cmake_minimum_required(VERSION 3.22.0)

message(STATUS "${CMAKE_SYSTEM}")
message(STATUS "cmake ${CMAKE_VERSION}")
message(STATUS "\tcommand: ${CMAKE_COMMAND}")
message(STATUS "\ttoolchain: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "\tgenerator: ${CMAKE_GENERATOR}")
message(STATUS "\tbuild tool: ${CMAKE_BUILD_TOOL}")
message(STATUS "compiler")
message(STATUS "\t${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "\tcommand: ${CMAKE_CXX_COMPILER}")
message(STATUS "build_type: ${CMAKE_BUILD_TYPE}")
message(STATUS "paths")
message(STATUS "\tinstall: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "\tsource: ${PROJECT_SOURCE_DIR}")
message(STATUS "\tbinary: ${PROJECT_BINARY_DIR}")
message(STATUS)

project(autodis VERSION 0.1.0)

add_subdirectory(shared)
add_subdirectory(keeper)
add_subdirectory(collector)

add_executable(autodis
	main.cpp
)

target_compile_features(autodis
    PUBLIC
        cxx_std_23
)

target_precompile_headers(autodis
	PUBLIC
		pch.h
)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(autodis PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(autodis PRIVATE nlohmann_json::nlohmann_json)
