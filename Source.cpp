#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <stdio.h>

#include <librealsense2\rs.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION // Header file for writing PNG files
#include <stb_image_write.h>

#include "displayfunctions.hpp" // For creating GLFW Window
#include "imguifunctions.hpp" // For drawing ImGui menus
#include "imguitheme.hpp"  // For changing ImGui themes
#include "opencvfunctions.hpp" // For selecting points on an image and getting coordinates
#include "converter.hpp" // For converting .bag files
#include "converter-csv.hpp" // For converting .bag to .csv files
#include "converter-png.hpp" // For converting .bag to .png files
#include "csvmanager.hpp" // For managing 921,600 (x, y, z) points

int main(int argc, char* argv[]) {
	// OpenGL + GLFW + ImGui Declarations
	window_rs window; // Create window object from Class window_rs
	window.onCreate(1280, 720, "WetlingRealsenseES V2.5"); // Creates a 1280x720 OpenGL window
	ImGui_ImplGlfw_Init(window, true); // Initalize ImGui with callbacks enabled

	imguithemes::windowsTheme(); // Changing ImGui theme values
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear();
	ImFont* font_small = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 19.0f);
	font_small->DisplayOffset.y -= 1;
	ImFont* font_large = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 25.0f);
	font_large->DisplayOffset.y -= 1;

	// Realsense Declarations
	texture_rs rgbPreview, depthPreview;
	rs2::frameset data;
	rs2::frame color, depth;
	rs2::colorizer color_map;
	rs2::temporal_filter tempFilter;
	rs2::hole_filling_filter holeFilter;
	std::vector<std::shared_ptr<rs2::tools::converter::converter_base>> convertToCSV;
	std::vector<std::shared_ptr<rs2::tools::converter::converter_base>> convertToPNG;
	auto pipe = std::make_shared<rs2::pipeline>(); // Realsense pipeline declaration
	rs2::pipeline pipe2;

	// Realsense Camera State Management
	bool isCameraOn = false; // Bool to check if Camera is on at any given point in time
	bool turnCameraOnRequest = false; // Bool to set when camera is needed to be on for various functions

	// Realsense Pipeline Configurations
	//rs2::config convertConfig; // Pipeline configuration declaration when converting .bag files

	rs2::config standardConfig; // Pipeline configuration declaration when performing normal streaming
	standardConfig.enable_stream(RS2_STREAM_COLOR, -1, 1280, 720, rs2_format::RS2_FORMAT_RGB8, 0);
	standardConfig.enable_stream(RS2_STREAM_DEPTH, -1, 1280, 720, rs2_format::RS2_FORMAT_Z16, 0);

	rs2::config recordConfig; // Pipeline configuration declaration when performing .bag recording
	recordConfig.enable_record_to_file("record.bag");
	recordConfig.enable_stream(RS2_STREAM_COLOR, -1, 1280, 720, rs2_format::RS2_FORMAT_RGB8, 0);
	recordConfig.enable_stream(RS2_STREAM_DEPTH, -1, 1280, 720, rs2_format::RS2_FORMAT_Z16, 0);

	// File management Declarations
	std::string fileName, bagName;
	std::ifstream importVerticesFile;

	// ImGui state Declarations
	bool show_color_camera = false; // Stream Color
	bool show_depth_camera = false; // Stream Depth
	bool camera_button_png = false; // Capture Color & Depth PNG
	bool camera_button_rosbag = false; // Capture 1s ROSBAG
	bool convert_bag_button = false; // Generate CSV File with ROSBAG
	bool coordinates_button = false; // Select Coordinates in a PNG
	bool import_csv_button = false; // Import 921,600 points for volume calculation

	bool rosbag_menu_display_done = false; // Bool condition to manage closing of Rosbag menu
	bool csv_menu_display_done = false; // Bool condition to manage closing of CSV menu
	bool opencv_menu_display_done = false; // Bool condition to manage closing of OpenCV menu

	// ImGui Display Variables
	char bagFileBuffer[64] = ""; // Buffer to store .bag file name
	char newFileBuffer[64] = ""; // Buffer to store file name to be saved
	char csvFileBuffer[64] = ""; // Buffer to store .csv file name
	char opencvFileBuffer[64] = ""; // Buffer to store .png file name for OpenCV

	while (window) { // From window_rs class, calls operator bool() every loop
		ImGui_ImplGlfw_NewFrame(1);
		ImGuiFunctions::menuGUI(font_large, font_small, show_color_camera, show_depth_camera, camera_button_png, camera_button_rosbag, convert_bag_button, coordinates_button, import_csv_button); // Creates and displays main menu

		char csvFileBuffer[64] = "TestXResults_Depth_35.csv";
		import_csv_button = true;

		// Checks if streaming condition is set
		if (show_color_camera == true || show_depth_camera == true) { // Check if color or depth streaming is enabled
			turnCameraOnRequest = true; // Sets request to turn camera on

			if (isCameraOn == true && turnCameraOnRequest == true) { // If camera needs to be on, and is already on, do nothing
				//std::cout << "Camera Already On!" << std::endl;
			}
			else if (isCameraOn == false && turnCameraOnRequest == true) { // If camera needs to be on, but it is not on, start pipeline
				pipe->start(standardConfig); // Start Realsense pipeline with standard configuration
				isCameraOn = true; // Sets camera condition as on
			}

			data = pipe->wait_for_frames().apply_filter(holeFilter); // Wait for frames for pipeline and apply hole filling filter
			color = data.get_color_frame(); // Get color frame from data frameset
			depth = color_map.process(data.get_depth_frame()); // Get depth frame from data frameset

			if (show_color_camera) { // If color streaming is enabled, render color frames
				rgbPreview.render(color, { 640, 0, window.width() / 2, window.height() / 2 });
			}

			if (show_depth_camera) { // If depth streaming is enabled, render depth frames
				depthPreview.render(depth, { 640, 360, window.width() / 2, window.height() / 2 });
			}
		}
		else { // If color or depth streaming is not enabled, set request to turn camera on to false
			turnCameraOnRequest = false;
		}

		if (camera_button_png) { // Set if camera png capture is pressed
			turnCameraOnRequest = true; // Sets request to turn camera on

			if (isCameraOn == true && turnCameraOnRequest == true) { // If camera needs to be on, and is already on, do nothing
				//std::cout << "Camera Already On!" << std::endl;
			}
			else if (isCameraOn == false && turnCameraOnRequest == true) { // If camera needs to be on, but it is not on, start pipeline
				pipe->start(standardConfig); // Start Realsense pipeline with standard configuration
				isCameraOn = true; // Sets camera condition as on
			}

			for (auto i = 0; i < 20; ++i) { // Capture 20 frames for camera hardware to settle
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
			turnCameraOnRequest = false;
			camera_button_png = false;
		}

		if (camera_button_rosbag) { // TODO: Fix bag unindexed error
			bool previousCameraOn = isCameraOn;
			turnCameraOnRequest = true;

			if (isCameraOn == true && turnCameraOnRequest == true) { // If camera needs to be on, and is already on, do nothing
				pipe->stop();
				pipe->start(recordConfig);
				isCameraOn = true;
			}
			else if (isCameraOn == false && turnCameraOnRequest == true) {
				pipe->start(recordConfig);
				//pipe2.start(recordConfig);
				isCameraOn = true;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
			pipe->stop(); // Stop the pipeline that holds the file and the recorder
			std::cout << "Pipe Stopped (ROSBAG)" << std::endl;
			//pipe.reset();
			std::cout << "Pipe Reset (ROSBAG)" << std::endl;

			if (previousCameraOn == true) {
				pipe->start(standardConfig); // Resume streaming with default configuration
				isCameraOn = true;
			}
			else {
				isCameraOn = false;
			}
			
			turnCameraOnRequest = false;
			camera_button_rosbag = false;
		}

		if (convert_bag_button) { // TODO: Fix Realsense Pipeline
			//ImGuiFunctions::rosbagGUI(rosbag_menu_display_done, bagFileBuffer, newFileBuffer); // Creates and displays rosbag menu

			//if (rosbag_menu_display_done) {
			if (1) {
				//convertToCSV.push_back(std::make_shared<rs2::tools::converter::converter_csv>(newFileBuffer));
				//convertToPNG.push_back(std::make_shared<rs2::tools::converter::converter_png>(newFileBuffer));
				//convertConfig.enable_device_from_file(bagFileBuffer);
				rs2::config convertConfig;
				convertToCSV.push_back(std::make_shared<rs2::tools::converter::converter_csv>("abc"));
				convertToPNG.push_back(std::make_shared<rs2::tools::converter::converter_png>("abc"));
				convertConfig.enable_device_from_file("record.bag");

				if (isCameraOn) {
					std::cout << "Camera On" << std::endl;
					pipe->stop();
					std::cout << "Camera Off" << std::endl;
					isCameraOn = false;
				}
				else {
					std::cout << "Before record config start" << std::endl;
				}
				
				pipe->start(convertConfig);
				std::cout << "After record config start" << std::endl;
				auto device = pipe->get_active_profile().get_device();
				rs2::playback playback = device.as<rs2::playback>();
				playback.set_real_time(false);
				auto duration = playback.get_duration();
				int progress = 0;
				int frameNumber = 0ULL;
				std::cout << "After load of shit init" << std::endl;

				while (true) {
					std::cout << "Waiting for frames" << std::endl;
					auto frameset_c = pipe->wait_for_frames();
					std::cout << frameset_c[0].get_frame_number() << std::endl;

					if (frameset_c[0].get_frame_number() > 5) { // If end of frame, bag loops again, hence get_frame_number resets to 1 and is smaller
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
				//pipe->start(standardConfig);

				rosbag_menu_display_done = false;
				convert_bag_button = false;
			}
		}

		if (coordinates_button) {
			ImGuiFunctions::opencvGUI(opencv_menu_display_done, opencvFileBuffer); // Creates and displays OpenCV menu

			if (opencv_menu_display_done) {
				OpenCVFunctions::selectCoordinates(opencvFileBuffer);

				opencv_menu_display_done = false;
				coordinates_button = false;
			}
		}

		if (import_csv_button) {
			char eater;
			int xs, ys;
			Array2D<double, 1280, 720> vertices;

			int numberOfLines = 0;
			std::string line;

			char dump;
			std::string temp;

			std::string a, b;

			std::ifstream interestPointsFile;
			interestPointsFile.open("interestPointsFile.csv");
			if (!interestPointsFile.is_open()) {
				std::cout << "ERROR IN OPENING INTERESTPOINTSFILE.CSV" << std::endl;
			}

			ImGuiFunctions::csvGUI(csv_menu_display_done, csvFileBuffer);

			if (csv_menu_display_done) {
				importVerticesFile.open(csvFileBuffer);
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

				/*
				for (int a{}; a < 5; a++) {
					std::cout << "Enter X val: ";
					std::cin >> xs;
					std::cout << "Enter Y val: ";
					std::cin >> ys;

					std::cout << "Value = " << vertices(xs - 1, ys - 1) << std::endl << std::endl;
				}
				*/

				std::vector<int> xCoords;
				std::vector<int> yCoords;

				while (interestPointsFile.good()) {
					std::getline(interestPointsFile, a, ',');
					std::getline(interestPointsFile, b, '\n');

					xCoords.push_back(std::stoi(a));
					yCoords.push_back(std::stoi(b));
					//std::cout << a << " " << b << std::endl;
					numberOfLines++;
					std::cout << numberOfLines << std::endl;
				}

				std::cout << xCoords.at(0) << " " << yCoords.at(0) << std::endl;
				std::cout << xCoords.at(1) << " " << yCoords.at(1) << std::endl;
				std::cout << xCoords.at(2) << " " << yCoords.at(2) << std::endl;
				std::cout << xCoords.at(3) << " " << yCoords.at(3) << std::endl;
				//std::cout << xCoords[0] << " " << yCoords[0] << std::endl;
				//std::cout << xCoords[1] << " " << yCoords[1] << std::endl;
				//std::cout << xCoords[2] << " " << yCoords[2] << std::endl;
				//std::cout << xCoords[3] << " " << yCoords[3] << std::endl;

				interestPointsFile.close();
				importVerticesFile.close();

				csv_menu_display_done = false;
				import_csv_button = false;
			}
		}

		if (isCameraOn == true && turnCameraOnRequest == false) {
			pipe->stop();
			std::cout << "Pipe Stopped" << std::endl;
			isCameraOn = false;
		}
		ImGui::Render(); // Render ImGui at the end of every loop
	}
}
