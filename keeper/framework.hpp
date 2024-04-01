#pragma once

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <limits>
#include <span>
#include <string>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <shared/shared.hpp>
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>

using namespace std::literals;
