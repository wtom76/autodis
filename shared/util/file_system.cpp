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
//---------------------------------------------------------------------------------------------------------
bool shared::util::not_exist_or_overwrite(std::filesystem::path const& out_path)
{
	if (!std::filesystem::exists(out_path))
	{
		return true;
	}
	std::cout << "file '" << out_path.native() << "' already exists. overwrite? (y/n)" << std::endl;
	char answer{0};
	std::cin >> answer;
	return answer == 'y' || answer == 'Y';
}
