cmake_minimum_required(VERSION 3.22.0)

add_library(feed OBJECT
	finam_daily_csv.cpp
	linear_generator.cpp
	moex_rest.cpp
	metastock.cpp
)

target_include_directories(feed PRIVATE
	"./"
	${CMAKE_SOURCE_DIR}
)

target_compile_features(feed PRIVATE cxx_std_23)

if(MSVC)
	target_compile_options(feed PRIVATE /W4 /WX)
else()
	target_compile_options(feed PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_precompile_headers(feed PRIVATE pch.hpp)

find_package(libpqxx CONFIG REQUIRED)
target_link_libraries(feed PRIVATE libpqxx::pqxx)

find_package(spdlog CONFIG REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)

find_package(pugixml CONFIG REQUIRED)
target_link_libraries(feed PRIVATE pugixml pugixml::static pugixml::pugixml)
