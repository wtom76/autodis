#pragma once

#include <algorithm>
#include <cassert>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <initializer_list>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <functional>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <stdexcept>
#include <sstream>
#include <unordered_set>
#include <vector>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/fmt/ostr.h>

#include <nlohmann/json.hpp>

using namespace std::literals;