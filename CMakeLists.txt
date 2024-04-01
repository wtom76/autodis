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
add_subdirectory(feature)
add_subdirectory(collector)
add_subdirectory(learning)
add_subdirectory(model)
add_subdirectory(visual)

add_executable(autodis
	config.cpp
	application.cpp
	main.cpp
)

target_compile_features(autodis PRIVATE cxx_std_23)

if(MSVC)
	target_compile_options(autodis PRIVATE /W4 /WX)
else()
	target_compile_options(autodis PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(autodis PUBLIC pch.hpp)

target_link_libraries(autodis PRIVATE shared)
target_link_libraries(autodis PRIVATE keeper)
target_link_libraries(autodis PRIVATE feature)
target_link_libraries(autodis PRIVATE learning)
target_link_libraries(autodis PRIVATE model)
target_link_libraries(autodis PRIVATE visual)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(autodis PRIVATE nlohmann_json::nlohmann_json)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(autodis PRIVATE spdlog::spdlog)

find_package(libpqxx CONFIG REQUIRED)
target_link_libraries(autodis PRIVATE libpqxx::pqxx)

find_package(matplot++ CONFIG REQUIRED)
target_link_libraries(autodis PRIVATE Matplot++::cimg std::filesystem Matplot++::matplot Matplot++::nodesoup)

target_include_directories(autodis PRIVATE ${CMAKE_SOURCE_DIR})
