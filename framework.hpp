#pragma once

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <stdexcept>
#include <thread>
#include <vector>
#include <unordered_map>
#include <utility>

using namespace std::literals;

#include <shared/shared.hpp>
#include <keeper/keeper.hpp>
#include <learning/learning.hpp>
#include <visual/chart.hpp>
