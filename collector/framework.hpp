#pragma once

#include <span>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <charconv>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <iostream> // temp for test

using namespace std::literals;

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <shared/shared.hpp>
