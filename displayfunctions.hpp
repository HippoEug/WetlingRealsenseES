#pragma once

#include <librealsense2\rs.hpp>

#define GLFW_INCLUDE_GLU
#include <GLFW\glfw3.h>

#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw.h"

// Basic Data Types, where Struct behaves like a Class except members and base classes are public by default
struct float3_rs {
	float x, y, z;
};

struct float2_rs {
	float x, y;
};

struct rect_rs {
	float x, y;
	float w, h;

	// Create new rect within original boundaries with give aspect ration
	rect_rs adjust_ratio(float2_rs size) const {
		auto H = static_cast<float>(h), W = static_cast<float>(h) * size.x / size.y;
		if (W > w) {
			auto scale = w / W;
			W *= scale;
			H *= scale;
		}

		return{ x + (w - W) / 2, y + (h - H) / 2, W, H };
	}
};

// Font Loading
#include "realsense\stb_easy_font.h"

inline void draw_text_rs(int x, int y, const char * text) {
	char buffer[60000]; // ~300 chars
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 16, buffer);
	glDrawArrays(GL_QUADS, 0, 4 * stb_easy_font_print((float)x, (float)(y - 7), (char *)text, nullptr, buffer, sizeof(buffer)));
	glDisableClientState(GL_VERTEX_ARRAY);
}

// Image Display Code
class texture_rs {
public:
	void render(const rs2::frameset& frames, int window_width, int window_height) {
		std::vector<rs2::video_frame> supported_frames;

		for (auto f : frames) {
			if (can_render(f))
				supported_frames.push_back(f);
		}

		if (supported_frames.empty()) {
			return;
		}

		std::sort(supported_frames.begin(), supported_frames.end(), [](rs2::frame first, rs2::frame second) {
			return first.get_profile().stream_type() < second.get_profile().stream_type();
		});

		auto image_grid = calc_grid(float2_rs{ (float)window_width, (float)window_height }, supported_frames);

		int image_index = 0;
		for (auto f : supported_frames) {
			upload(f);
			show(image_grid.at(image_index));
			image_index++;
		}
	}

	void render(const rs2::video_frame& frame, const rect_rs& r) {
		upload(frame);
		show(r.adjust_ratio({ float(width), float(height) }));
	}

	void upload(const rs2::video_frame& frame) {
		if (!frame) {
			return;
		}

		if (!gl_handle) {
			glGenTextures(1, &gl_handle);
		}
		GLenum err = glGetError();

		auto format = frame.get_profile().format();
		width = frame.get_width();
		height = frame.get_height();
		stream = frame.get_profile().stream_type();

		glBindTexture(GL_TEXTURE_2D, gl_handle);

		switch (format) {
		case RS2_FORMAT_RGB8:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.get_data());
			break;
		case RS2_FORMAT_RGBA8:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.get_data());
			break;
		case RS2_FORMAT_Y8:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.get_data());
			break;
		default:
			throw std::runtime_error("The requested format is not supported by this demo!");
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint get_gl_handle() {
		return gl_handle;
	}

	void show(const rect_rs& r) const {
		if (!gl_handle) {
			return;
		}

		glBindTexture(GL_TEXTURE_2D, gl_handle);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUAD_STRIP);
		glTexCoord2f(0.f, 1.f); glVertex2f(r.x, r.y + r.h);
		glTexCoord2f(0.f, 0.f); glVertex2f(r.x, r.y);
		glTexCoord2f(1.f, 1.f); glVertex2f(r.x + r.w, r.y + r.h);
		glTexCoord2f(1.f, 0.f); glVertex2f(r.x + r.w, r.y);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		draw_text_rs((int)r.x + 15, (int)r.y + 20, rs2_stream_to_string(stream));
	}

private:
	GLuint gl_handle = 0;
	int width = 0;
	int height = 0;
	rs2_stream stream = RS2_STREAM_ANY;

	bool can_render(const rs2::frame& f) const {
		auto format = f.get_profile().format();
		switch (format) {
		case RS2_FORMAT_RGB8:
		case RS2_FORMAT_RGBA8:
		case RS2_FORMAT_Y8:
			return true;
		default:
			return false;
		}
	}

	rect_rs calc_grid(float2_rs window, int streams) {
		if (window.x <= 0 || window.y <= 0 || streams <= 0) {
			throw std::runtime_error("invalid window configuration request, failed to calculate window grid");
		}

		float ratio = window.x / window.y;
		auto x = sqrt(ratio * (float)streams);
		auto y = (float)streams / x;
		auto w = round(x);
		auto h = round(y);

		if (w == 0 || h == 0) {
			throw std::runtime_error("invalid window configuration request, failed to calculate window grid");
		}

		while (w*h > streams) {
			if (h > w) {
				h--;
			}
			else {
				w--;
			}
		}

		while (w*h < streams) {
			if (h > w) {
				w++;
			}
			else {
				h++;
			}
		}

		auto new_w = round(window.x / w);
		auto new_h = round(window.y / h);
		// column count, line count, cell width cell height
		return rect_rs{ static_cast<float>(w), static_cast<float>(h), static_cast<float>(new_w), static_cast<float>(new_h) };
	}

	std::vector<rect_rs> calc_grid(float2_rs window, std::vector<rs2::video_frame>& frames) {
		auto grid = calc_grid(window, frames.size());

		int index = 0;
		std::vector<rect_rs> rv;
		int curr_line = -1;

		for (auto f : frames) {
			auto mod = index % (int)grid.x;
			auto fw = (float)frames[index].get_width();
			auto fh = (float)frames[index].get_height();

			float cell_x_postion = (float)(mod * grid.w);
			if (mod == 0) curr_line++;
			float cell_y_position = curr_line * grid.h;

			auto r = rect_rs{ cell_x_postion, cell_y_position, grid.w, grid.h };
			rv.push_back(r.adjust_ratio(float2_rs{ fw, fh }));
			index++;
		}

		return rv;
	}
};

class window_rs
{
public:
	std::function<void(bool)>           on_left_mouse = [](bool) {};
	std::function<void(double, double)> on_mouse_scroll = [](double, double) {};
	std::function<void(double, double)> on_mouse_move = [](double, double) {};
	std::function<void(int)>            on_key_release = [](int) {};

	void onCreate(int width, int height, const char* title) {
		_width = width;
		_height = height;

		// Initialize the library
		if (!glfwInit()) {
			throw std::runtime_error("GLFW INIT FAILED");
			return;
		}

		// Create a windowed mode window and the OpenGL context
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Prevent window from resizing by user
		win = glfwCreateWindow(width, height, title, nullptr, nullptr);
		// Create a full screen window, no border or decorations
		// win = glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), NULL);

		if (!win) {
			glfwTerminate();
			throw std::runtime_error("Could not open OpenGL window, please check your graphic drivers or use the textual SDK tools");
		}

		glfwMakeContextCurrent(win); // Make the window's context current
		glfwSetWindowUserPointer(win, this); // Sets user-defined pointer of the specified window

		glfwSetMouseButtonCallback(win, [](GLFWwindow * win, int button, int action, int mods) { // Notify when a mouse button is pressed or released
			auto s = (window_rs*)glfwGetWindowUserPointer(win);
			if (button == 0) s->on_left_mouse(action == GLFW_PRESS);
		});

		glfwSetScrollCallback(win, [](GLFWwindow * win, double xoffset, double yoffset) { // Notify when user scrolls, with mouse wheel or touchpad gesture
			auto s = (window_rs*)glfwGetWindowUserPointer(win);
			s->on_mouse_scroll(xoffset, yoffset);
		});

		glfwSetCursorPosCallback(win, [](GLFWwindow * win, double x, double y) { // Notify when cursor is moved over the window
			auto s = (window_rs*)glfwGetWindowUserPointer(win);
			s->on_mouse_move(x, y);
		});

		glfwSetKeyCallback(win, [](GLFWwindow * win, int key, int scancode, int action, int mods) { // Notify when a physical key is pressed, released, or repeated
			auto s = (window_rs*)glfwGetWindowUserPointer(win);
			if (0 == action) { // on key release
				s->on_key_release(key);
			}
		});
	}

	float width() const {
		return float(_width);
	}

	float height() const {
		return float(_height);
	}

	operator bool() {
		glPopMatrix();
		// GLFW windows are double buffered by default, a front buffer and a back buffer
		// Front buffer is the one being displayed, while the back buffer is the one being rendered
		// When the entire frame has been rendered, it is time to swap the back and the front buffers,
		// in order to display what has been rendered and begin rendering a new frame
		glfwSwapBuffers(win); // Swap front and back buffers
		glfwSwapInterval(1); // VSync, FPS Limit: 60

		auto res = !glfwWindowShouldClose(win);

		// Event Processing: GLFW needs to communicate with the system to receive events and show the application has not locked up
		// glfwPollEvents processes only those events that have already been received and then returns immediately
		glfwPollEvents(); // Poll for and process events
		glfwGetFramebufferSize(win, &_width, &_height); // For directly retrieving the current size of the framebuffer of aindow

														// Clear the framebuffer
		glClear(GL_COLOR_BUFFER_BIT); // Render here
		glViewport(0, 0, _width, _height);

		// Draw the images
		glPushMatrix();
		glfwGetWindowSize(win, &_width, &_height);
		glOrtho(0, _width, _height, 0, -1, +1);

		return res;
	}

	~window_rs() {
		glfwDestroyWindow(win); // Window destroyed when no longer needed
		glfwTerminate(); // Terminates te GLFW library, destroying any remaining window, monitor or objects etc
	}

	operator GLFWwindow*() {
		return win;
	}

private:
	GLFWwindow * win;
	int _width, _height;
};
