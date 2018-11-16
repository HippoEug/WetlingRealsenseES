#pragma once

#include "imgui\imgui.h"

namespace ImGuiFunctions {
	void menuGUI(bool& show_color_camera, bool& show_depth_camera, bool& camera_button_png, bool& camera_button_rosbag, bool& convert_bag_button, bool& coordinates_button);
	void rosbagGUI();
}
