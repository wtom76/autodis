#include "pch.h"
#include "source/file.h"
#include "feed/finam_daily_file.h"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		return 0;
	}
	collector::source::file src{argv[1]};
	collector::feed::finam_daily_file dest;
	src.fetch(dest);
}