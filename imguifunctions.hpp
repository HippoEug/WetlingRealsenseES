#pragma once
#include "imgui\imgui.h"

namespace ImGuiFunctions {
	void menuGUI(ImFont* font_large, ImFont* font_small, bool& show_color_camera, bool& show_depth_camera, bool& camera_button_png, bool& camera_button_rosbag, bool& convert_bag_button, bool& coordinates_button, bool& import_csv_button);
	void rosbagGUI(bool& rosbag_menu_display_done, char bagFileBuffer[64], char newFileBuffer[64]);
	void csvGUI(bool& csv_menu_display_done, char csvFileBuffer[64]);
	void opencvGUI(bool& opencv_menu_display_done, char opencvFileBuffer[64]);
}
