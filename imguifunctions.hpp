#pragma once

#include "imgui\imgui.h"

namespace ImGuiFunctions {
	void menuGUI(bool& show_color_camera, bool& show_depth_camera, bool& camera_button, bool& coordinates_button, bool& CSV_button);
	void rosbagGUI();
}
