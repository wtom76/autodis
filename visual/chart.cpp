#include "pch.hpp"
#include "chart.hpp"
#include "shader.hpp"

namespace autodis::visual
{
	void throw_on_fail(GLenum err)
	{
		if (err != GLEW_OK)
		{
			throw std::runtime_error{"OpenGL error"};
		}
	}
}
namespace autodis::visual::prototypes
{
	//---------------------------------------------------------------------------------------------------------
	class test_draw
	{
		GLuint vao_{0};
		std::array<GLfloat, 9> const vertex_buffer_data_{
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			0.0f,  1.0f, 0.0f,
		};
		GLuint vertex_buffer_{0};
		GLuint program_id_{0};

	public:
		test_draw()
		{
			throw_on_fail(glewInit());
			glGenVertexArrays(1, &vao_);
			glBindVertexArray(vao_);

			// Generate 1 buffer, put the resulting identifier in vertexbuffer
			glGenBuffers(1, &vertex_buffer_);
			// The following commands will talk about our 'vertexbuffer' buffer
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
			// Give our vertices to OpenGL.
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_buffer_data_.size(), vertex_buffer_data_.data(), GL_STATIC_DRAW);

			// Create and compile our GLSL program from the shaders
			program_id_ = load_shaders("visual/SimpleVertexShader.vertexshader", "visual/SimpleFragmentShader.fragmentshader");
		}
		void draw()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Use our shader
			glUseProgram(program_id_);

			// 1st attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
			glDisableVertexAttribArray(0);
		}
	};
	//---------------------------------------------------------------------------------------------------------
	class test_draw_candle
	{
		GLuint vao_{0};
		std::array<GLfloat, 24> const vertex_buffer_data_{
			-.5f, -.5f,
			-.5f, .5f,
			.0f, -.5f,
			.0f, .5f,
			.5f, -.5f,
			.5f, .5f,
			// triangles
			-.47f, -.3f,	// br
			-.47f, .3f,		// tr
			-.53f, -.3f,	// bl
			-.53f, -.3f,	// bl
			-.53f, .3f,		// tl
			-.47f, .3f		// tr
		};
		GLuint vertex_buffer_{0};

	public:
		test_draw_candle()
		{
			throw_on_fail(glewInit());
			glGenVertexArrays(1, &vao_);
			glBindVertexArray(vao_);

			// Generate 1 buffer, put the resulting identifier in vertexbuffer
			glGenBuffers(1, &vertex_buffer_);
			// The following commands will talk about our 'vertexbuffer' buffer
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
			// Give our vertices to OpenGL.
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_buffer_data_.size(), vertex_buffer_data_.data(), GL_STATIC_DRAW);
		}
		void draw()
		{
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			// 1st attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				2,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				nullptr             // array buffer offset
			);

			glDrawArrays(GL_LINE_STRIP, 0, 2);
			glDrawArrays(GL_LINE_STRIP, 2, 2);
			glDrawArrays(GL_LINE_STRIP, 4, 2);
			glDrawArrays(GL_TRIANGLES, 6, 3);
			glDrawArrays(GL_TRIANGLES, 9, 3);
			glDisableVertexAttribArray(0);
		}
	};
} // ~namespace autodis::visual::prototypes
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
class autodis::visual::chart::chart::gl_context
{
// types
private:
	enum class chart_type
	{
		null = 0,
		line,
		candles
	};

	struct chart_meta
	{
		chart_type	type_{chart_type::null};
		size_t		points_num_{0};
	};

// data
private:
	std::vector<chart_meta> charts_;

	GLuint vao_{0};
	GLuint vertex_buffer_id_{0};
	std::vector<float> vertices_;
	
	GLuint color_buffer_id_{0};
	std::vector<float> color_buffer_;

	GLuint program_id_{0};

public:
	static constexpr size_t dimentions_{2};
	static constexpr size_t vertices_in_candle_{6};

// methods
private:
	size_t _draw_line(size_t first_idx, size_t points_num);
	size_t _draw_candles(size_t first_idx, size_t points_num);

public:
	gl_context()
	{
		throw_on_fail(glewInit());
		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		program_id_ = load_shaders("visual/SimpleVertexShader.vertexshader", "visual/SimpleFragmentShader.fragmentshader");
	}
	~gl_context()
	{
		glDeleteBuffers(1, &vertex_buffer_id_);
		glDeleteBuffers(1, &color_buffer_id_);
		glDeleteProgram(program_id_);
		glDeleteVertexArrays(1, &vao_);
	}

	std::vector<float>& vertices() noexcept { return vertices_; }
	std::vector<float>& color_buffer() noexcept { return color_buffer_; }
	void add_line(size_t points_num) { charts_.emplace_back(chart_type::line, points_num); }
	void add_candles(size_t points_num) { charts_.emplace_back(chart_type::candles, points_num); }
	void draw();
};
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::chart::gl_context::draw()
{
	glGenBuffers(1, &vertex_buffer_id_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_.size(), vertices_.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &color_buffer_id_);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * color_buffer_.size(), color_buffer_.data(), GL_STATIC_DRAW);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(program_id_);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		nullptr             // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id_);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		nullptr                           // array buffer offset
	);

	size_t current_point_idx{0};
	for (auto const& chart : charts_)
	{
		switch (chart.type_)
		{
		case chart_type::line:
			current_point_idx = _draw_line(current_point_idx, chart.points_num_);
			break;
		case chart_type::candles:
			current_point_idx = _draw_candles(current_point_idx, chart.points_num_);
			break;
		default:
			assert(false);
			break;
		}
	}

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}
//---------------------------------------------------------------------------------------------------------
size_t autodis::visual::chart::chart::gl_context::_draw_line(size_t first_idx, size_t points_num)
{
	glDrawArrays(GL_LINE_STRIP, first_idx, points_num);
	return first_idx + points_num;
}
//---------------------------------------------------------------------------------------------------------
size_t autodis::visual::chart::chart::gl_context::_draw_candles(size_t first_idx, size_t points_num)
{
	assert(points_num % 6 == 0);

	size_t idx_end{first_idx + points_num};
	for (size_t vidx{first_idx}; vidx != idx_end;)
	{
		glDrawArrays(GL_LINE_STRIP, vidx, 2);
		vidx += 2;
		glDrawArrays(GL_TRIANGLES, vidx, 3);
		vidx += 1;
		glDrawArrays(GL_TRIANGLES, vidx, 3);
		vidx += 3;
	}
	return idx_end;
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
autodis::visual::chart::chart::chart(shared::data::frame const& df)
	: df_{df}
	, scale_x_{df}
{}
//---------------------------------------------------------------------------------------------------------
autodis::visual::chart::chart::~chart()
{
	_wait();
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::_wait()
{
	try
	{
		if (fut_.valid())
		{
			fut_.get();
		}
	}
	catch (std::exception const& ex)
	{
		SPDLOG_LOGGER_ERROR(log(), ex.what());
	}
}
//---------------------------------------------------------------------------------------------------------
shared::math::range autodis::visual::chart::_min_max(std::array<size_t, 4> const& ohlc_idc) const noexcept
{
	return {
		shared::math::min(df_.series(ohlc_idc[2])),
		shared::math::max(df_.series(ohlc_idc[1]))
	};
}
//---------------------------------------------------------------------------------------------------------
std::unique_ptr<autodis::visual::chart::gl_context> autodis::visual::chart::_create_ctx() const
{
	std::unique_ptr<autodis::visual::chart::gl_context> ctx{std::make_unique<gl_context>()};
	for (auto const& candles : candlesticks_)
	{
		ctx->add_candles(_add_candles(candles, ctx->vertices(), ctx->color_buffer()));
	}
	for (auto const& line : lines_)
	{
		ctx->add_line(_add_line(line, ctx->vertices(), ctx->color_buffer()));
	}
	return ctx;
}
//---------------------------------------------------------------------------------------------------------
size_t autodis::visual::chart::_add_line(line const& src, std::vector<float>& dst_verices, std::vector<float>& dst_colors) const
{
	constexpr size_t dimentions{gl_context::dimentions_};

	constexpr std::array<float, 3> color{.5f, 0.5f, 0.f};

	dst_verices.reserve(dst_verices.size() + dimentions * df_.row_count());
	dst_colors.reserve(dst_colors.size() + color.size() * df_.row_count());

	data_frame_t::series_t const& series{df_.series(src.series_idx_)};

	scale_y const& scl_y{scales_y_[src.scale_y_idx_]};

	for (size_t row{scale_x_.first_visible_idx()}; row != df_.row_count(); ++row)
	{
		float const x{scale_x_.position(row)};
		float const y{scl_y.position(series[row])};

		dst_verices.emplace_back(x);
		dst_verices.emplace_back(y);

		dst_colors.emplace_back(color[0]);
		dst_colors.emplace_back(color[1]);
		dst_colors.emplace_back(color[2]);
	}
	return df_.row_count() - scale_x_.first_visible_idx();
}
//---------------------------------------------------------------------------------------------------------
size_t autodis::visual::chart::_add_candles(candles const& src, std::vector<float>& dst_verices, std::vector<float>& dst_colors) const
{
	constexpr size_t dimentions{gl_context::dimentions_};
	constexpr size_t vertices_in_candle{gl_context::vertices_in_candle_};

	constexpr std::array<float, 3> green{0.f, 1.f, 0.f};
	constexpr std::array<float, 3> red{1.f, 0.f, 0.f};

	dst_verices.reserve(dst_verices.size() + dimentions * vertices_in_candle * df_.row_count());
	dst_colors.reserve(dst_colors.size() + 3 * vertices_in_candle * df_.row_count());

	data_frame_t::series_t const& open_s{df_.series(src.ohlc_[0])};
	data_frame_t::series_t const& high_s{df_.series(src.ohlc_[1])};
	data_frame_t::series_t const& low_s{df_.series(src.ohlc_[2])};
	data_frame_t::series_t const& close_s{df_.series(src.ohlc_[3])};
	float const candle_half_width{scale_x_.step() / 4.f};

	scale_y const& scl_y{scales_y_[src.scale_y_idx_]};

	for (size_t row{scale_x_.first_visible_idx()}; row != df_.row_count(); ++row)
	{
		float const x{scale_x_.position(row)};
		float const y_open {scl_y.position(open_s[row])};
		float const y_high {scl_y.position(high_s[row])};
		float const y_low  {scl_y.position(low_s[row])};
		float const y_close{scl_y.position(close_s[row])};

		// wick top
		dst_verices.emplace_back(x);
		dst_verices.emplace_back(y_high);
		// wick bottom
		dst_verices.emplace_back(x);
		dst_verices.emplace_back(y_low);

		float const body_top{std::max(y_open, y_close)};
		float const body_bottom{std::min(y_open, y_close)};

		// triangle 1
		// top left
		dst_verices.emplace_back(x - candle_half_width);
		dst_verices.emplace_back(body_top);
		// top right
		dst_verices.emplace_back(x + candle_half_width);
		dst_verices.emplace_back(body_top);
		// bottom left
		dst_verices.emplace_back(x - candle_half_width);
		dst_verices.emplace_back(body_bottom);
		// triangle 2
		// top right ^
		// bottom left ^
		// bottom right
		dst_verices.emplace_back(x + candle_half_width);
		dst_verices.emplace_back(body_bottom);

		std::array<float, 3> const& color{open_s[row] <= close_s[row] ? green : red};
		for (int i{6}; i; --i)
		{
			dst_colors.emplace_back(color[0]);
			dst_colors.emplace_back(color[1]);
			dst_colors.emplace_back(color[2]);
		}
	}
	return (df_.row_count() - scale_x_.first_visible_idx()) * 6;
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::_add_scale_y(size_t scale_y_idx, shared::math::range rng)
{
	if (scale_y_idx >= max_scales_y_)
	{
		assert(false);
		return;
	}
	if (scale_y_idx >= scales_y_.size())
	{
		scales_y_.resize(scale_y_idx + 1);
	}
	scales_y_[scale_y_idx].update(rng);
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::add_line(size_t scale_y_idx, size_t series_idx)
{
	_add_scale_y(scale_y_idx, shared::math::range::min_max(df_.series(series_idx)));
	lines_.emplace_back(series_idx, scale_y_idx);
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::add_candlesticks(size_t scale_y_idx, std::array<size_t, 4> const& ohlc_idc)
{
	_add_scale_y(scale_y_idx, _min_max(ohlc_idc));
	candlesticks_.emplace_back(ohlc_idc, scale_y_idx);
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::show()
{
	fut_ = std::async(std::launch::async,
		[this]()
		{
			window_.create(sf::VideoMode{800, 600}, "Test window");
			window_.setVerticalSyncEnabled(true);
			window_.setKeyRepeatEnabled(false);

			gl_ctx_ = _create_ctx();

			while (window_.isOpen())
			{
				sf::Event event;
				while (window_.waitEvent(event))
				{
					switch (event.type)
					{
					case sf::Event::Closed:
						window_.close();
						break;
					case sf::Event::Resized:
						glViewport(0, 0, event.size.width, event.size.height);
						//window_.SetActive();
						gl_ctx_->draw();
						window_.display();
						break;
					default:
						break;
					}
				}
			}
		}
	);
}