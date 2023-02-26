#pragma once

#include <span>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <utility>

#include <pqxx/pqxx>
#include <nlohmann/json.hpp>

using namespace std::literals;

#include <shared/shared.hpp>