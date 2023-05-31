#include "pch.hpp"
#include "file_system.hpp"

//---------------------------------------------------------------------------------------------------------
std::string shared::util::exe_name()
{
	return std::filesystem::canonical("/proc/self/exe"s).stem();
}
//---------------------------------------------------------------------------------------------------------
std::filesystem::path shared::util::exe_dir_path()
{
	return std::filesystem::canonical("/proc/self/exe"s).parent_path();
}
//---------------------------------------------------------------------------------------------------------
std::filesystem::path shared::util::log_root()
{
	return std::filesystem::canonical(std::getenv("HOME")) / "log"s;
}
