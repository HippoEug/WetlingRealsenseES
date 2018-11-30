#include <chrono>
#include <ctime>
#include <string>

#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw.h"

#include "imguifunctions.hpp"

void ImGuiFunctions::menuGUI(ImFont* font_large, ImFont* font_small, bool& show_color_camera, bool& show_depth_camera, bool& camera_button_png, bool& camera_button_rosbag, bool& convert_bag_button, bool& coordinates_button, bool& import_csv_button) {
	static const int flags = ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
	ImGui::Begin("Menu", nullptr, flags);
	ImGui::SetCursorPos({ 300, 5 });

	ImGui::Separator();
	ImGui::PushFont(font_large);
	ImGui::Text(" Stream");
	ImGui::PopFont();
	ImGui::PushFont(font_small);
	ImGui::Separator();
	ImGui::Checkbox("Preview Color Camera", &show_color_camera);
	ImGui::Checkbox("Preview Depth Camera", &show_depth_camera);
	ImGui::PopFont();

	ImGui::Separator();
	ImGui::PushFont(font_large);
	ImGui::Text(" Save");
	ImGui::PopFont();
	ImGui::Separator();
	ImGui::PushFont(font_small);
	if (ImGui::Button("Capture Image", { 150, 35 })) {
		camera_button_png = true;
	}
	if (ImGui::Button("Capture Rosbag", { 150, 35 })) {
		camera_button_rosbag = true;
	}
	ImGui::PopFont();

	ImGui::Separator();
	ImGui::PushFont(font_large);
	ImGui::Text(" Msc' Functions");
	ImGui::Separator();
	ImGui::PopFont();
	ImGui::PushFont(font_small);
	if (ImGui::Button("Convert ROSBAG", { 150, 35 })) {
		convert_bag_button = true;
	}
	if (ImGui::Button("Select Coordinates", { 150, 35 })) {
		coordinates_button = true;
	}
	if (ImGui::Button("Import CSV", { 150, 35 })) {
		import_csv_button = true;
	}
	ImGui::PopFont();

	ImGui::Separator();
	ImGui::PushFont(font_large);
	ImGui::Text(" Log");
	ImGui::PopFont();
	ImGui::Separator();

	ImGui::Text("");
	ImGui::Text("");
	ImGui::Text("");
	ImGui::Text("");
	ImGui::Text("");
	ImGui::Text("");
	ImGui::Text("");
	ImGui::Separator();
	ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);
	std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(p);
	ImGui::Text(ctime(&t));
	ImGui::Separator();
	ImGui::Text("HippoEug, 2018. Version 2.4.1");
	ImGui::Separator();

	ImGui::End();
	ImGui::PopStyleVar();
}

void ImGuiFunctions::rosbagGUI(bool& rosbag_menu_display_done, char bagFileBuffer[64], char newFileBuffer[64]) {
	static const int flags = ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoResize;

	ImGui::SetNextWindowPos({ 440, 260 });
	ImGui::Begin("Rosbag Converter", nullptr, flags);
	ImGui::SetCursorPos({ 400, 10 });
	ImGui::Text("");
	ImGui::Text("Enter the .bag File Name: ");
	ImGui::InputText("", bagFileBuffer, 64);
	ImGui::Text("Enter File Name to Save: ");
	ImGui::InputText(" ", newFileBuffer, 64);

	ImGui::Text("");
	ImGui::Separator();
	/*
	if (ImGui::Button("CANCEL")) {
		// ADD CANCEL BUTTON
	}
	*/
	if (ImGui::Button("DONE")) {
		rosbag_menu_display_done = true;
	}

	ImGui::End();
}

void ImGuiFunctions::csvGUI(bool& csv_menu_display_done, char csvFileBuffer[64]) {
	static const int flags = ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoResize;

	ImGui::SetNextWindowPos({ 440, 260 });
	ImGui::Begin("CSV Menu Display", nullptr, flags);
	ImGui::SetCursorPos({ 400, 10 });
	ImGui::Text("");
	ImGui::Text("Enter the .csv File Name: ");
	ImGui::InputText("  ", csvFileBuffer, 64);

	ImGui::Text("");
	ImGui::Separator();
	/*
	if (ImGui::Button("CANCEL")) {
		// ADD CANCEL BUTTON
	}
	*/
	if (ImGui::Button("DONE")) {
		csv_menu_display_done = true;
	}

	ImGui::End();
}

void ImGuiFunctions::opencvGUI(bool& opencv_menu_display_done, char opencvFileBuffer[64]) {
	static const int flags = ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoResize;

	ImGui::SetNextWindowPos({ 440, 260 });
	ImGui::Begin("OpenCV Menu Display", nullptr, flags);
	ImGui::SetCursorPos({ 400, 10 });
	ImGui::Text("");
	ImGui::Text("Enter the .png File Name: ");
	ImGui::InputText("   ", opencvFileBuffer, 64);

	ImGui::Text("");
	ImGui::Separator();
	/*
	if (ImGui::Button("CANCEL")) {
		// ADD CANCEL BUTTON
	}
	*/
	if (ImGui::Button("DONE")) {
		opencv_menu_display_done = true;
	}

	ImGui::End();
}
