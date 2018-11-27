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
#include "csvmanager.hpp" // Uncomment this, not documented in Report/Github yet

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int argc, char* argv[]) {
	// OpenGL + GLFW + ImGui Declarations
	window_rs window; // Create window object from Class window_rs
	window.onCreate(1280, 720, "WetlingRealsenseES V1.1"); // Creates a 1280x720 OpenGL window
	ImGui_ImplGlfw_Init(window, false); // Initalize ImGui

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

	char buf1[64] = "Test Display";

	pipe->start(standardConfig); // Start Realsense pipeline

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear();

	ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	font->DisplayOffset.y -= 1;

	ImGuiStyle* style = &ImGui::GetStyle();
	int hspacing = 8;
	int vspacing = 6;
	style->DisplaySafeAreaPadding = ImVec2(0, 0);
	style->WindowPadding = ImVec2(hspacing / 2, vspacing);
	style->FramePadding = ImVec2(hspacing, vspacing);
	style->ItemSpacing = ImVec2(hspacing, vspacing);
	style->ItemInnerSpacing = ImVec2(hspacing, vspacing);
	style->IndentSpacing = 20.0f;

	style->WindowRounding = 0.0f;
	style->FrameRounding = 0.0f;

	//style->WindowBorderSize = 0.0f;
	//style->FrameBorderSize = 1.0f;
	//style->PopupBorderSize = 1.0f;

	style->ScrollbarSize = 20.0f;
	style->ScrollbarRounding = 0.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 0.0f;

	ImVec4 white = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	ImVec4 transparent = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	ImVec4 dark = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
	ImVec4 darker = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);

	ImVec4 background = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
	ImVec4 text = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	ImVec4 border = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	ImVec4 grab = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	ImVec4 header = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	ImVec4 active = ImVec4(0.00f, 0.47f, 0.84f, 1.00f);
	ImVec4 hover = ImVec4(0.00f, 0.47f, 0.84f, 0.20f);

	style->Colors[ImGuiCol_Text] = text;
	style->Colors[ImGuiCol_WindowBg] = background;
	//style->Colors[ImGuiCol_ChildBg] = background;
	style->Colors[ImGuiCol_PopupBg] = white;

	style->Colors[ImGuiCol_Border] = border;
	style->Colors[ImGuiCol_BorderShadow] = transparent;

	style->Colors[ImGuiCol_Button] = header;
	style->Colors[ImGuiCol_ButtonHovered] = hover;
	style->Colors[ImGuiCol_ButtonActive] = active;

	style->Colors[ImGuiCol_FrameBg] = white;
	style->Colors[ImGuiCol_FrameBgHovered] = hover;
	style->Colors[ImGuiCol_FrameBgActive] = active;

	style->Colors[ImGuiCol_MenuBarBg] = header;
	style->Colors[ImGuiCol_Header] = header;
	style->Colors[ImGuiCol_HeaderHovered] = hover;
	style->Colors[ImGuiCol_HeaderActive] = active;

	style->Colors[ImGuiCol_CheckMark] = text;
	style->Colors[ImGuiCol_SliderGrab] = grab;
	style->Colors[ImGuiCol_SliderGrabActive] = darker;

	style->Colors[ImGuiCol_CloseButton] = transparent;
	style->Colors[ImGuiCol_CloseButtonHovered] = transparent;
	style->Colors[ImGuiCol_CloseButtonActive] = transparent;

	style->Colors[ImGuiCol_ScrollbarBg] = header;
	style->Colors[ImGuiCol_ScrollbarGrab] = grab;
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = dark;
	style->Colors[ImGuiCol_ScrollbarGrabActive] = darker;

	/*
	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiIO& io = ImGui::GetIO();
	style.Colors[ImGuiCol_Text] = ImVec4(0.31f, 0.25f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.74f, 0.74f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.68f, 0.68f, 0.68f, 0.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.62f, 0.70f, 0.72f, 0.56f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.33f, 0.14f, 0.47f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.97f, 0.31f, 0.13f, 0.81f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.42f, 0.75f, 1.00f, 0.53f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.65f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.40f, 0.62f, 0.80f, 0.15f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.39f, 0.64f, 0.80f, 0.30f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.67f, 0.80f, 0.59f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.48f, 0.53f, 0.67f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.89f, 0.98f, 1.00f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.48f, 0.47f, 0.47f, 0.71f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.31f, 0.47f, 0.99f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(1.00f, 0.79f, 0.18f, 0.78f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.42f, 0.82f, 1.00f, 0.81f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.72f, 1.00f, 1.00f, 0.86f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.65f, 0.78f, 0.84f, 0.80f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.75f, 0.88f, 0.94f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.55f, 0.68f, 0.74f, 0.80f);//ImVec4(0.46f, 0.84f, 0.90f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.60f, 0.60f, 0.80f, 0.30f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.41f, 0.75f, 0.98f, 0.50f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(1.00f, 0.47f, 0.41f, 0.60f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(1.00f, 0.16f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.99f, 0.54f, 0.43f);;
	*/

	while (window) {
		ImGui_ImplGlfw_NewFrame(1);
		ImGuiFunctions::menuGUI(show_color_camera, show_depth_camera, camera_button_png, camera_button_rosbag, convert_bag_button, coordinates_button);

		// START EDIT 27112018
		//ImGui::Begin("Rosbag Converter");
		//ImGui::Text("Enter .bag File Name: ");
		//ImGui::InputText("default", buf1, 64);
		//ImGui::End();
		// END EDIT 27112018

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
			//std::cout << "WAT" << std::endl;
			ImGuiFunctions::rosbagGUI(rosbag_menu_display_done, buf1);

			if (rosbag_menu_display_done) {
				//std::cout << "Enter File Name: ";
				//std::cin >> fileName;
				//std::cout << "Enter Bag Name: ";
				//std::cin >> bagName;

				//convertToCSV.push_back(std::make_shared<rs2::tools::converter::converter_csv>(fileName));
				//convertToPNG.push_back(std::make_shared < rs2::tools::converter::converter_png>(fileName));
				//convertConfig.enable_device_from_file(bagName);

				convertToCSV.push_back(std::make_shared<rs2::tools::converter::converter_csv>("realsense"));
				convertToPNG.push_back(std::make_shared < rs2::tools::converter::converter_png>("realsense"));
				convertConfig.enable_device_from_file("record.bag");

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
