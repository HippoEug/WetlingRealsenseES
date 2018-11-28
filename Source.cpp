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
#include "converter.hpp"
#include "converter-csv.hpp"
#include "converter-png.hpp"
#include "csvmanager.hpp"
#include "imguitheme.hpp"

int main(int argc, char* argv[]) {
	// OpenGL + GLFW + ImGui Declarations
	window_rs window; // Create window object from Class window_rs
	window.onCreate(1280, 720, "WetlingRealsenseES V2.3"); // Creates a 1280x720 OpenGL window
	ImGui_ImplGlfw_Init(window, true); // Initalize ImGui
	imguithemes::windowsTheme();

	// Realsense Declarations
	texture_rs rgbPreview, depthPreview;
	rs2::frameset data;
	rs2::frame color, depth;
	rs2::colorizer color_map;
	rs2::temporal_filter tempFilter;
	rs2::hole_filling_filter holeFilter;
	std::vector<std::shared_ptr<rs2::tools::converter::converter_base>> convertToCSV;
	std::vector<std::shared_ptr<rs2::tools::converter::converter_base>> convertToPNG;
	auto pipe = std::make_shared<rs2::pipeline>();

	// Realsense Pipeline Configurations
	rs2::config convertConfig;
	//convertConfig.enable_device_from_file("record.bag");

	rs2::config standardConfig;
	standardConfig.enable_stream(RS2_STREAM_COLOR, -1, 1280, 720, rs2_format::RS2_FORMAT_RGB8, 0);
	standardConfig.enable_stream(RS2_STREAM_DEPTH, -1, 1280, 720, rs2_format::RS2_FORMAT_Z16, 0);

	rs2::config recordConfig;
	recordConfig.enable_record_to_file("record.bag");
	recordConfig.enable_stream(RS2_STREAM_COLOR, -1, 1280, 720, rs2_format::RS2_FORMAT_RGB8, 0);
	recordConfig.enable_stream(RS2_STREAM_DEPTH, -1, 1280, 720, rs2_format::RS2_FORMAT_ANY, 0);

	// File management Declarations
	std::string fileName, bagName;
	std::ifstream importVerticesFile;
	std::ofstream randomTestFile;
	randomTestFile.open("randomTestFile.csv");

	// ImGui state Declarations
	bool show_color_camera = false; // Stream Color
	bool show_depth_camera = false; // Stream Depth
	bool camera_button_png = false; // Capture Color & Depth PNG
	bool camera_button_rosbag = false; // Capture 1s ROSBAG TODO
	bool convert_bag_button = false; // Generate CSV File with ROSBAG TODO
	bool coordinates_button = false; // Select Coordinates in a PNG
	bool import_csv_button = false;
	bool rosbag_menu_display_done = false;

	// ImGui Display Variables
	char bagFileBuffer[64] = "1";
	char newFileBuffer[64] = "2";

	pipe->start(standardConfig); // Start Realsense pipeline

	while (window) {
		ImGui_ImplGlfw_NewFrame(1);
		ImGuiFunctions::menuGUI(show_color_camera, show_depth_camera, camera_button_png, camera_button_rosbag, convert_bag_button, coordinates_button);

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

		if (camera_button_png) {
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

			camera_button_png = false;
		}

		if (camera_button_rosbag) {
			pipe->stop();
			pipe->start(recordConfig);

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));

			pipe->stop(); // Stop the pipeline that holds the file and the recorder
			pipe->start(standardConfig); // Resume streaming with default configuration

			camera_button_rosbag = false;
		}

		if (convert_bag_button) {
			ImGuiFunctions::rosbagGUI(rosbag_menu_display_done, bagFileBuffer, newFileBuffer);

			if (rosbag_menu_display_done) {
				convertToCSV.push_back(std::make_shared<rs2::tools::converter::converter_csv>(newFileBuffer));
				convertToPNG.push_back(std::make_shared < rs2::tools::converter::converter_png>(newFileBuffer));
				convertConfig.enable_device_from_file(bagFileBuffer);

				pipe->stop();
				pipe->start(convertConfig);
				auto device = pipe->get_active_profile().get_device();
				rs2::playback playback = device.as<rs2::playback>();
				playback.set_real_time(false);
				auto duration = playback.get_duration();
				int progress = 0;
				int frameNumber = 0ULL;

				while (true) {
					auto frameset_c = pipe->wait_for_frames();

					if (frameset_c[0].get_frame_number() == 5 || frameset_c[0].get_frame_number() == 6 || frameset_c[0].get_frame_number() == 7) { // If end of frame, bag loops again, hence get_frame_number resets to 1 and is smaller
						std::for_each(convertToCSV.begin(), convertToCSV.end(), [&frameset_c](std::shared_ptr<rs2::tools::converter::converter_base>& converter) {
							converter->convert(frameset_c);
						});
						std::cout << "\nCONVERTING CSV" << std::endl;
						std::for_each(convertToCSV.begin(), convertToCSV.end(), [](std::shared_ptr<rs2::tools::converter::converter_base>& converter) {
							converter->wait();
						});
						std::cout << "CONVERTING PICTURES" << std::endl;
						std::for_each(convertToPNG.begin(), convertToPNG.end(), [&frameset_c](std::shared_ptr<rs2::tools::converter::converter_base>& converter) {
							converter->convert(frameset_c);
						});
						std::for_each(convertToPNG.begin(), convertToPNG.end(), [](std::shared_ptr<rs2::tools::converter::converter_base>& converter) {
							converter->wait();
						});
						break;
					}

					frameNumber = frameset_c[0].get_frame_number();
				}

				std::cout << "\nDONE!" << std::endl << std::endl;

				pipe->stop();
				std::cout << "HERE" << std::endl;
				pipe->start(standardConfig);

				rosbag_menu_display_done = false;
				convert_bag_button = false;
			}
		}

		if (coordinates_button) {
			selectCoordinates();

			coordinates_button = false;
		}

		if (import_csv_button) {
			char eater;
			int xs, ys;
			Array2D<double, 1280, 720> vertices;

			importVerticesFile.open("realsense_Depth_5.csv");
			std::cout << "COPYING FROM CSV FILE" << std::endl;
			for (int y{}; y < 720; ++y) {
				for (int x{}; x < 1280; ++x) {
					if (!(importVerticesFile >> vertices(x, y) >> std::noskipws >> eater >> std::skipws) && !importVerticesFile.eof() && eater != ',' && eater != '\n') {
						std::cerr << "Format error at " << x + 1 << '/' << y + 1 << " :(\n\n";
						return EXIT_FAILURE;
					}
				}
			}
			std::cout << "DONE" << std::endl;

			for (int a{}; a < 5; a++) {
				std::cout << "Enter X val: ";
				std::cin >> xs;
				std::cout << "Enter Y val: ";
				std::cin >> ys;

				std::cout << "Value = " << vertices(xs - 1, ys - 1) << std::endl << std::endl;
			}

			import_csv_button = false;
		}

		ImGui::Render();
	}
}
