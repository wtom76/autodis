#include "pch.hpp"
#include "chart.hpp"
#include "shader.hpp"

void throw_on_fail(GLenum err)
{
	if (err != GLEW_OK)
	{
		throw std::runtime_error{"OpenGL error"};
	}
}

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
		program_id_ = LoadShaders("visual/SimpleVertexShader.vertexshader", "visual/SimpleFragmentShader.fragmentshader");
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
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
class autodis::visual::chart::chart::gl_context
{
	GLuint vao_{0};
	GLuint vertex_buffer_{0};
	std::vector<float> vertices_;

public:
	static constexpr size_t dimentions_{2};
	static constexpr size_t vertices_in_candle_{6};

public:
	gl_context()
	{
		throw_on_fail(glewInit());
		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);
		glGenBuffers(1, &vertex_buffer_);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
	}

	std::vector<float>& vertices() noexcept { return vertices_; }
	void draw();
};
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::chart::gl_context::draw()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_.size(), vertices_.data(), GL_STATIC_DRAW);

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

	for (size_t vidx{0}; vidx < vertices_.size();)
	{
		glDrawArrays(GL_LINE_STRIP, vidx, 2);
		vidx += 2;
		glDrawArrays(GL_TRIANGLES, vidx, 3);
		vidx += 1;
		glDrawArrays(GL_TRIANGLES, vidx, 3);
		vidx += 3;
	}
	glDisableVertexAttribArray(0);
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
		_add_candles(candles, ctx->vertices());
	}
	return ctx;
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::_add_candles(candles const& src, std::vector<float>& dst) const
{
	constexpr size_t dimentions{gl_context::dimentions_};
	constexpr size_t vertices_in_candle{gl_context::vertices_in_candle_};

	dst.reserve(dst.size() + dimentions * vertices_in_candle * df_.row_count());

	data_frame_t::series_t const& open_s{df_.series(src.ohlc_[0])};
	data_frame_t::series_t const& high_s{df_.series(src.ohlc_[1])};
	data_frame_t::series_t const& low_s{df_.series(src.ohlc_[2])};
	data_frame_t::series_t const& close_s{df_.series(src.ohlc_[3])};
	float const candle_half_width{scale_x_.step() / 4.f};

	size_t row{0};
	for (; row != df_.row_count() && df_.index()[row] < scale_x_.first_visible_value(); ++row)
	{}
	for (; row != df_.row_count(); ++row)
	{
		float const x{scale_x_.position(row)};
		float const y_open {src.scale_y_.position(open_s[row])};
		float const y_high {src.scale_y_.position(high_s[row])};
		float const y_low  {src.scale_y_.position(low_s[row])};
		float const y_close{src.scale_y_.position(close_s[row])};

		// wick top
		dst.emplace_back(x);
		dst.emplace_back(y_high);
		// wick bottom
		dst.emplace_back(x);
		dst.emplace_back(y_low);

		float const body_top{std::max(y_open, y_close)};
		float const body_bottom{std::min(y_open, y_close)};

		// triangle 1
		// top left
		dst.emplace_back(x - candle_half_width);
		dst.emplace_back(body_top);
		// top right
		dst.emplace_back(x + candle_half_width);
		dst.emplace_back(body_top);
		// bottom left
		dst.emplace_back(x - candle_half_width);
		dst.emplace_back(body_bottom);
		// triangle 2
		// top right ^
		// bottom left ^
		// bottom right
		dst.emplace_back(x + candle_half_width);
		dst.emplace_back(body_bottom);
	}
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::set_candlesticks(std::array<size_t, 4> const& ohlc_idc)
{
	candlesticks_.emplace_back(ohlc_idc, _min_max(ohlc_idc));
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