#pragma once
#include <opencv2/opencv.hpp>

namespace OpenCVFunctions {
	struct mouse_state
	{
		mouse_state()
			: position(-1, -1)
			, left_button_held(false)
			, left_button_clicked(false)
		{}

		void new_iteration()
		{
			left_button_clicked = false;
		}

		cv::Point2i position; // Last position where the LMB was down
		bool left_button_held; // Is the LMB down right now?
		bool left_button_clicked; // Was the LMB down in the last iteration?
	};

	void selectCoordinates(char opencvFileBuffer[64]);
	void mouse_callback(int event, int x, int y, int flag, void* param);
}
