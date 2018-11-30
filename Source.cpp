#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <stdio.h>

#include <librealsense2\rs.hpp>
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION // Header file for writing PNG files
#include "realsense\stb_image_write.h"

#include "displayfunctions.hpp" // For creating GLFW Window
#include "imguifunctions.hpp" // For drawing ImGui menus
#include "opencvfunctions.hpp" // For selecting points on an image and getting coordinates

int main(int argc, char* argv[]) {
	window_rs window; // Create window object from Class window_rs
	window.onCreate(1280, 720, "WetlingRealsenseES V1.1"); // Creates a 1280x720 OpenGL window
	ImGui_ImplGlfw_Init(window, false); // Initalize ImGui

	texture_rs rgbPreview, depthPreview;
	rs2::frameset data;
	rs2::frame color, depth;
	rs2::colorizer color_map;
	rs2::temporal_filter tempFilter;
	rs2::hole_filling_filter holeFilter;
	auto pipe = std::make_shared<rs2::pipeline>();

	rs2::config standardConfig;
	standardConfig.enable_stream(RS2_STREAM_COLOR, -1, 1280, 720, rs2_format::RS2_FORMAT_RGB8, 0);
	standardConfig.enable_stream(RS2_STREAM_DEPTH, -1, 1280, 720, rs2_format::RS2_FORMAT_ANY, 0);

	std::ofstream csvDataFile;
	csvDataFile.open("csvDataFile.csv");
	std::string fileName, bagName;

	// Booleans to control ImGui
	bool show_color_camera = false; // Stream Color
	bool show_depth_camera = false; // Stream Depth
	bool camera_button = false;
	bool coordinates_button = false;
	bool CSV_button = false;

	pipe->start(standardConfig); // Start Realsense pipeline

	while (window) {
		ImGui_ImplGlfw_NewFrame(1);
		ImGuiFunctions::menuGUI(show_color_camera, show_depth_camera, camera_button, coordinates_button, CSV_button);

		if (show_color_camera == true || show_depth_camera == true) {
			data = pipe->wait_for_frames().apply_filter(holeFilter);
			color = data.get_color_frame();
			depth = color_map.process(data.get_depth_frame());

			if (show_color_camera) {
				rgbPreview.render(color, { 640, 0, window.width() / 2, window.height() / 2 });
			}

			if (show_depth_camera) {
				depthPreview.render(depth, { 640, 360, window.width() / 2, window.height() / 2 });
			}
		}

		if (camera_button) {
			for (auto i = 0; i < 10; ++i) { // Capture 10 frames for camera hardware to settle
				pipe->wait_for_frames();
			}

			for (auto&& frame : pipe->wait_for_frames()) { // Get next set of frames from the camera
				// We can only save video frames as pngs, so we skip the rest
				if (auto vf = frame.as<rs2::video_frame>()) {
					auto stream = frame.get_profile().stream_type();
					// Use the colorizer to get an rgb image for the depth stream
					if (vf.is<rs2::depth_frame>()) vf = color_map.process(frame);

					// Write images to disk
					std::stringstream png_file;
					png_file << "capture-" << vf.get_profile().stream_name() << ".png";
					stbi_write_png(png_file.str().c_str(), vf.get_width(), vf.get_height(), vf.get_bytes_per_pixel(), vf.get_data(), vf.get_stride_in_bytes());
					std::cout << "Saved " << png_file.str() << std::endl;
				}
			}

			camera_button = false;
		}

		if (coordinates_button) {
			selectCoordinates();
			coordinates_button = false;
		}

		if (CSV_button) {
			rs2::frameset frames;
			rs2::frame depth;

			pipe->stop();
			rs2::config cfg;
			cfg.enable_record_to_file("test.bag");
			pipe->start(cfg);

			// capture 1 frame here?

			pipe->stop(); // Stop the pipeline that holds the file and the recorder
			pipe = std::make_shared<rs2::pipeline>(); //Reset the shared pointer with a new pipeline
			pipe->start(); // Resume streaming with default configuration

			CSV_button = false;
		}
	}
}
