#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <limits>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <stdexcept>
#include <sstream>
#include <vector>

//#pragma warning (push)
//#pragma warning (disable: 4459) // conflict with "integral"
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/fmt/ostr.h>
//#pragma warning (pop)

using namespace std::literals;