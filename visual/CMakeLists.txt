cmake_minimum_required(VERSION 3.22.0)

add_library(visual OBJECT
	scale_date.cpp
	scale_y.cpp
	chart.cpp
	shader.cpp
)

target_include_directories(visual PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(visual PRIVATE cxx_std_23)

if(MSVC)
	target_compile_options(visual PRIVATE /W4 /WX)
else()
	target_compile_options(visual PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(visual PRIVATE pch.hpp)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(visual PRIVATE spdlog::spdlog)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(visual PRIVATE nlohmann_json::nlohmann_json)

find_package(SFML COMPONENTS system window graphics REQUIRED)
target_link_libraries(visual PRIVATE sfml-graphics)

find_package(GLEW REQUIRED)
target_link_libraries(visual PRIVATE GLEW::GLEW)
