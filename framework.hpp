#pragma once

#include <cassert>
#include <cmath>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>

using namespace std::literals;

#include <shared/shared.hpp>
#include <keeper/keeper.hpp>
#include <learning/learning.hpp>
#include <visual/chart.hpp>
#include <pqxx/pqxx>
