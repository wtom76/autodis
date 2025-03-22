#include "pch.hpp"
#include "chart.hpp"
#include "shader.hpp"
#include "util.hpp"

// TODO: use buffers swaping

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
	void _release_buffers();
	void _reset_buffers();
	size_t _draw_line(size_t first_idx, size_t points_num);
	size_t _draw_candles(size_t first_idx, size_t points_num);

public:
	gl_context()
	{
		throw_on_fail(glewInit());
		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		program_id_ = load_shaders("visual/SimpleVertexShader.vertexshader", "visual/SimpleFragmentShader.fragmentshader");
		glUseProgram(program_id_);
	}
	~gl_context()
	{
		glDeleteProgram(program_id_);
		_release_buffers();
		glDeleteVertexArrays(1, &vao_);
	}

	void clear();
	std::vector<float>& vertices() noexcept { return vertices_; }
	std::vector<float>& color_buffer() noexcept { return color_buffer_; }

	void add_line(size_t points_num) { charts_.emplace_back(chart_type::line, points_num); }
	void add_candles(size_t points_num) { charts_.emplace_back(chart_type::candles, points_num); }
	void draw();
};
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::chart::gl_context::_release_buffers()
{
	if (color_buffer_id_)
	{
		glDeleteBuffers(1, &color_buffer_id_);
		color_buffer_id_ = 0;
	}
	if (vertex_buffer_id_)
	{
		glDeleteBuffers(1, &vertex_buffer_id_);
		vertex_buffer_id_ = 0;
	}
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::chart::gl_context::_reset_buffers()
{
	_release_buffers();

	glGenBuffers(1, &vertex_buffer_id_);
	assert(vertex_buffer_id_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_.size(), vertices_.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &color_buffer_id_);
	assert(color_buffer_id_);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * color_buffer_.size(), color_buffer_.data(), GL_STATIC_DRAW);
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
void autodis::visual::chart::chart::gl_context::clear()
{
	charts_.clear();
	vertices_.clear();
	color_buffer_.clear();
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::chart::gl_context::draw()
{
	_reset_buffers();

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

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
shared::math::min_max autodis::visual::chart::_min_max(std::array<size_t, 4> const& ohlc_idc) const noexcept
{
	return shared::math::min_max{
		shared::math::min(df_.series(ohlc_idc[2])),
		shared::math::max(df_.series(ohlc_idc[1]))
	};
}
//---------------------------------------------------------------------------------------------------------
std::unique_ptr<autodis::visual::chart::gl_context> autodis::visual::chart::_create_ctx() const
{
	return std::make_unique<gl_context>();
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::_fill_ctx(gl_context& ctx) const
{
	ctx.clear();

	// add y0-line
	{
		assert(!scales_y_.empty());
		assert(df_.row_count() > 0);
		float const y0{scales_y_.back().position(0.)};
		ctx.vertices().emplace_back(scale_x_.position(scale_x_.first_visible_idx()));
		ctx.vertices().emplace_back(y0);
		ctx.vertices().emplace_back(scale_x_.position(df_.row_count() - 1));
		ctx.vertices().emplace_back(y0);
		ctx.color_buffer().emplace_back(0.2f);
		ctx.color_buffer().emplace_back(0.2f);
		ctx.color_buffer().emplace_back(0.2f);
		ctx.color_buffer().emplace_back(0.2f);
		ctx.color_buffer().emplace_back(0.2f);
		ctx.color_buffer().emplace_back(0.2f);
		ctx.add_line(2);
	}

	for (auto const& candles : candlesticks_)
	{
		ctx.add_candles(_add_candles(candles, ctx.vertices(), ctx.color_buffer()));
	}
	for (auto const& line : lines_)
	{
		ctx.add_line(_add_line(line, ctx.vertices(), ctx.color_buffer()));
	}
}
//---------------------------------------------------------------------------------------------------------
size_t autodis::visual::chart::_add_line(line const& src, std::vector<float>& dst_verices, std::vector<float>& dst_colors) const
{
	constexpr size_t dimentions{gl_context::dimentions_};

	dst_verices.reserve(dst_verices.size() + dimentions * df_.row_count());
	dst_colors.reserve(dst_colors.size() + src.color_.size() * df_.row_count());

	data_frame_t::series_t const& series{df_.series(src.series_idx_)};

	scale_y const& scl_y{scales_y_[src.scale_y_idx_]};

	for (size_t row{scale_x_.first_visible_idx()}; row != df_.row_count(); ++row)
	{
		float const x{scale_x_.position(row)};
		float const y{scl_y.position(series[row])};

		dst_verices.emplace_back(x);
		dst_verices.emplace_back(y);

		dst_colors.emplace_back(src.color_[0]);
		dst_colors.emplace_back(src.color_[1]);
		dst_colors.emplace_back(src.color_[2]);
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

		assert(y_high >= y_low);

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
void autodis::visual::chart::_add_scale_y(size_t scale_y_idx, shared::math::min_max rng)
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
void autodis::visual::chart::add_line(size_t scale_y_idx, size_t series_idx, color_t color)
{
	_add_scale_y(scale_y_idx, shared::math::min_max(df_.series(series_idx)));
	lines_.emplace_back(series_idx, scale_y_idx, std::move(color));
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::add_candlesticks(size_t scale_y_idx, std::array<size_t, 4> const& ohlc_idc)
{
	_add_scale_y(scale_y_idx, _min_max(ohlc_idc));
	candlesticks_.emplace_back(ohlc_idc, scale_y_idx);
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::invalidate() noexcept
{
// DEBUG
	_dump_df(df_);
//~DEBUG

	need_recalc_.store(true);
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::chart::show()
{
	fut_ = std::async(std::launch::async,
		[this]()
		{
			window_.create(sf::VideoMode{sf::Vector2u{window_size_.x_, window_size_.y_}}, "Test window");
			window_.setVerticalSyncEnabled(true);
			window_.setKeyRepeatEnabled(false);

			gl_ctx_ = _create_ctx();
			_fill_ctx(*gl_ctx_);

			auto const on_close{
				[this](sf::Event::Closed const&)
				{
					window_.close();
				}
			};
			auto const on_resized{
				[this](sf::Event::Resized const& event)
				{
					window_size_.x_ = event.size.x;
					window_size_.y_ = event.size.y;
					glViewport(0, 0, event.size.x, event.size.y);
					need_redraw_.store(true);
				}
			};
			auto const on_mouse_button_pressed{
				[this](sf::Event::MouseButtonPressed const& event)
				{
					switch (event.button)
					{
					case sf::Mouse::Button::Left:
						mouse_.active_ = true;
						mouse_.current_point_.x_ = event.position.x;
						mouse_.current_point_.y_ = event.position.y;
						mouse_.start_point_ = mouse_.current_point_;
						scale_x_.start_scroll();
						break;
					default:
						break;
					}
				}
			};
			auto const on_mouse_button_released{
				[this](sf::Event::MouseButtonReleased const& event)
				{
					switch (event.button)
					{
					case sf::Mouse::Button::Left:
						mouse_.active_ = false;
						break;
					default:
						break;
					}
				}
			};
			auto const on_mouse_moved{
				[this](sf::Event::MouseMoved const& event)
				{
					if (mouse_.active_)
					{
						mouse_.current_point_.x_ = event.position.x;
						mouse_.current_point_.y_ = event.position.y;
						if (scale_x_.scroll(static_cast<float>(mouse_.current_point_.x_ - mouse_.start_point_.x_) / window_size_.x_))
						{
							need_recalc_.store(true);
						}
					}
				}
			};

			while (window_.isOpen())
			{
				window_.handleEvents(
					on_close,
					on_resized,
					on_mouse_button_pressed,
					on_mouse_button_released,
					on_mouse_moved
				);
				if (need_recalc_.exchange(false))
				{
					_fill_ctx(*gl_ctx_);
					need_redraw_.store(true);
				}
				if (need_redraw_.exchange(false))
				{
					gl_ctx_->draw();
					window_.display();
				}
				std::this_thread::sleep_for(10ms);
			}
		}
	);
}