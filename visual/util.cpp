#include "pch.hpp"
#include "util.hpp"

//---------------------------------------------------------------------------------------------------------
void autodis::visual::throw_on_fail(GLenum err)
{
	if (err != GLEW_OK)
	{
		throw std::runtime_error{"OpenGL error"};
	}
}
//---------------------------------------------------------------------------------------------------------
// DEBUG
void autodis::visual::_dump_df(shared::data::frame const& df)
{
	std::ofstream f{"df_chart.csv"s};
	df.print(f);
}
//~DEBUG
