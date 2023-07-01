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
	GLuint program_id_{0};

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

			test_draw_candle td;

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
						td.draw();
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