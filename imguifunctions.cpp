#include <chrono>
#include <ctime>
#include <string>

#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw.h"

#include "imguifunctions.hpp"

void ImGuiFunctions::menuGUI(bool& show_color_camera, bool& show_depth_camera, bool& camera_button_png, bool& camera_button_rosbag, bool& convert_bag_button, bool& coordinates_button) {
	static const int flags = ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove;

	ImGui::Begin("Menu", nullptr, flags);

	ImGui::Separator();
	ImGui::Text("-------STREAM-------");
	ImGui::Separator();
	ImGui::Checkbox("Preview Color Camera", &show_color_camera);
	ImGui::Checkbox("Preview Depth Camera", &show_depth_camera);

	ImGui::Separator();
	ImGui::Text("--------SAVE--------");
	ImGui::Separator();
	if (ImGui::Button("Capture Image", { 150, 25 })) {
		camera_button_png = true;
	}
	if (ImGui::Button("Capture Rosbag", { 150, 25 })) {
		camera_button_rosbag = true;
	}

	ImGui::Separator();
	ImGui::Text("---MSC' FUNCTIONS---");
	ImGui::Separator();
	if (ImGui::Button("Convert ROSBAG", { 150, 25 })) {
		convert_bag_button = true;
	}
	if (ImGui::Button("Select Coordinates", { 150, 25 })) {
		coordinates_button = true;
	}

	ImGui::Separator();
	ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);
	std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(p);
	ImGui::Text(ctime(&t));
	ImGui::Text("Copyright 2018, HippoEug. All Rights Reserved");
	ImGui::Separator();
	ImGui::End();
}

void ImGuiFunctions::rosbagGUI(bool& rosbag_menu_display_donechar, char bagFileBuffer[64], char newFileBuffer[64]) {
	ImGui::Begin("Rosbag Converter");
	ImGui::Text("Enter the .bag File Name: ");
	ImGui::InputText("", bagFileBuffer, 64);
	ImGui::Text("Enter File Name to Save: ");
	ImGui::InputText(" ", newFileBuffer, 64);

	if (ImGui::Button("DONE")) {
		rosbag_menu_display_donechar = true;
	}

	ImGui::End();
}
