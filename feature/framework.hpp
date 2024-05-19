#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <ostream>
#include <random>
#include <shared_mutex>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <string>
#include <unordered_map>
#include <vector>

#include <keeper/keeper.hpp>
#include <shared/shared.hpp>
#include <nlohmann/json.hpp>

#include <shared/data/frame.hpp>

using namespace std::literals;

namespace feature
{
	using feature_info_t	= keeper::metadata::feature_info;
	using index_pos_t		= std::ptrdiff_t;
	using index_value_t		= shared::data::frame::index_value_t;
	using value_t			= shared::data::frame::value_t;
}