#include "pch.hpp"
#include "moex_rest.hpp"

//---------------------------------------------------------------------------------------------------------
collector::feed::moex_rest::moex_rest()
{}
//---------------------------------------------------------------------------------------------------------
void collector::feed::moex_rest::start([[maybe_unused]] std::unique_ptr<keeper::data_write> dest)
{

}
//---------------------------------------------------------------------------------------------------------
size_t collector::feed::moex_rest::read([[maybe_unused]] std::span<const char> chunk)
{
	return 0;
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::moex_rest::finish([[maybe_unused]] std::span<const char> chunk)
{

}
