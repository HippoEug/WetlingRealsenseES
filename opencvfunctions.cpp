#include <fstream>

#include <opencv2/opencv.hpp>
#include "opencvfunctions.hpp"

void OpenCVFunctions::selectCoordinates(char opencvFileBuffer[64]) {
	std::ofstream interestPointsFile;
	interestPointsFile.open("interestPointsFile.csv");

	cv::String const WINDOW_NAME("Points Selection");
	cv::namedWindow(WINDOW_NAME, CV_WINDOW_AUTOSIZE);

	mouse_state ms;
	cv::setMouseCallback(WINDOW_NAME, mouse_callback, &ms);

	int const FRAME_WIDTH(1280);
	int const FRAME_HEIGHT(720);

	int previousX = 0;
	int previousY = 0;
	int newX = 0;
	int newY = 0;

	int diffX1 = 0;
	bool diffX1B = false;
	int diffX2 = 0;
	bool diffX2B = false;
	int diffY1 = 0;
	bool diffY1B = false;
	int diffY2 = 0;
	bool diffY2B = false;

	bool diffXNull = false;
	bool diffYNull = false;

	int clickCount = 0;
	int countA = 0;
	int ppy = 0;

	cv::Mat image;
	image = cv::imread(opencvFileBuffer);

	if (image.empty()) {
		std::cout << "Error: Image not found!" << std::endl;
		std::cerr << "Failed to read image";
	}

	while (1) {
		if (ms.left_button_clicked || ms.left_button_held) {
			if (clickCount == 0) { // If clicked for the first time
				previousX = ms.position.x - 1; // Set previousX as first position
				previousY = ms.position.y - 1; // Set previousY as first position

				newX = ms.position.x;
				newY = ms.position.y;
			}
			else {
				newX = ms.position.x;
				newY = ms.position.y;
			}
			clickCount++; // Increase clickCount each time there is a click

			if (newX == (previousX + 1) || newX == (previousX - 1) || newY == (previousY + 1) || newY == (previousY - 1)) { // If new points is beside old point by a pixel
				cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5); // Draw circle if next point is beside each other by a pixel
				interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
				previousX = newX; // Update new position as old position
				previousY = newY; // Update new position as old position
			}
			// else if (previousX != newX && previousY != newY)
			else { // If new point is not beside old point by a pixel
				if (newX > previousX) { // Find difference of points X; if new point is further to the right than older
					diffX1 = newX - previousX;
					diffX1B = true;
					//std::cout << "diffX 1 " << diffX << std::endl;
				}
				else if (previousX > newX) { // Find difference of points X; if new point is further to the left than older
					diffX2 = previousX - newX;
					diffX2B = true;
					//std::cout << "diffX 2 " << diffX << std::endl;
				}
				else if (previousX == newX) { // If there is no difference in pointsX, set difference to 0
					diffX1 = 0;
					diffX2 = 0;
					diffXNull = true;
				}

				if (newY > previousY) { // Find difference of points Y; if old point is lower than the older
					diffY1 = newY - previousY;
					diffY1B = true;
					//std::cout << "diffY 1 " << diffY << std::endl;
				}
				else if (previousY > newY) { // Find difference of points Y; if old point is higher than the new
					diffY2 = previousY - newY;
					diffY2B = true;
					//std::cout << "diffY 2 " << diffY << std::endl;
				}
				else if (previousY == newY) { // If there is no difference in pointsY, set difference to 0
					diffY1 = 0;
					diffY2 = 0;
					diffYNull = true;
				}

				if (diffX1B && diffY2B) { // To fill bottom-right
					for (int xToAdd = 0; xToAdd <= diffX1; xToAdd++) {
						ms.position.x = previousX + xToAdd; // changes
						ms.position.y = previousY; // doesn't change
						cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5);
						interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
					}
					for (int yToAdd = 0; yToAdd <= diffY2; yToAdd++) {
						ms.position.y = previousY - yToAdd;
						cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5);
						interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
					}
				}
				else if (diffX1B && diffY1B) { // To fill upper-right
					for (int xToAdd = 0; xToAdd <= diffX1; xToAdd++) {
						ms.position.x = previousX + xToAdd; // changes
						ms.position.y = previousY; // doesn't change
						cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5);
						interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
					}
					for (int yToAdd = 0; yToAdd <= diffY1; yToAdd++) {
						ms.position.y = previousY + yToAdd;
						cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5);
						interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
					}
				}
				else if (diffX2B && diffY2B) { // To fill bottom-left
					for (int xToAdd = 0; xToAdd <= diffX2; xToAdd++) {
						ms.position.x = previousX - xToAdd; // changes
						ms.position.y = previousY; // doesn't change
						cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5);
						interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
					}
					for (int yToAdd = 0; yToAdd <= diffY2; yToAdd++) {
						ms.position.y = previousY - yToAdd;
						cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5);
						interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
					}
				}
				else if (diffX2B && diffY1B) { // To fill upper-left
					for (int xToAdd = 0; xToAdd <= diffX2; xToAdd++) {
						ms.position.x = previousX - xToAdd; // changes
						ms.position.y = previousY; // doesn't change
						cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5);
						interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
					}
					for (int yToAdd = 0; yToAdd <= diffY1; yToAdd++) {
						ms.position.y = previousY + yToAdd;
						cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5);
						interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
					}
				}
				// STRAIGHT LINE HANDLING
				else if (diffXNull && diffY1B) { // VERTICAL UP
					for (int yToAdd = 0; yToAdd <= diffY1; yToAdd++) {
						ms.position.x = newX;
						ms.position.y = previousY + yToAdd;
						cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5);
						interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
					}
				}
				else if (diffXNull && diffY2B) { // VERTICAL DOWN
					for (int yToAdd = 0; yToAdd <= diffY2; yToAdd++) {
						ms.position.x = newX;
						ms.position.y = previousY - yToAdd;
						cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5);
						interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
					}
				}
				else if (diffX1B && diffYNull) { // HORIZONTAL RIGHT
					for (int xToAdd = 0; xToAdd <= diffX1; xToAdd++) {
						ms.position.x = previousX + xToAdd; // changes
						ms.position.y = newY; // doesn't change
						cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5);
						interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
					}
				}
				else if (diffX2B && diffYNull) { // HORIZONTAL LEFT
					for (int xToAdd = 0; xToAdd <= diffX2; xToAdd++) {
						ms.position.x = previousX - xToAdd; // changes
						ms.position.y = newY; // doesn't change
						cv::circle(image, ms.position, 1, cv::Scalar(0, 255, 83), 5);
						interestPointsFile << ms.position.x << "," << ms.position.y << std::endl;
					}
				}

				previousX = newX;
				previousY = newY;

				diffX1B = false;
				diffX2B = false;
				diffY1B = false;
				diffY2B = false;

				diffXNull = false;
				diffYNull = false;
			}
		}

		ms.new_iteration();

		cv::imshow(WINDOW_NAME, image);
		if (cv::waitKey(1) == 27) {
			std::cout << "Escaped Pressed, exiting..." << std::endl;
			break;
		}
	}

	interestPointsFile.close();
	cv::destroyAllWindows();
}

static void OpenCVFunctions::mouse_callback(int event, int x, int y, int flag, void* param) {
	mouse_state* state(static_cast<mouse_state*>(param));

	if (event == cv::EVENT_LBUTTONDOWN) {
		//std::cout << "LMB down @ (" << x << "," << y << ")\n";
		state->position = cv::Point2i(x, y);
		state->left_button_held = true;
	}
	else if (event == cv::EVENT_LBUTTONUP) {
		//std::cout << "LMB up @(" << x << "," << y << ")\n";
		state->position = cv::Point2i(x, y);
		state->left_button_held = false;
		state->left_button_clicked = true;
	}
	else if ((flag == cv::EVENT_FLAG_LBUTTON) && (event == cv::EVENT_MOUSEMOVE)) {
		//std::cout << "LMB held, mouse moved to (" << x << "," << y << ")\n";
		state->position = cv::Point2i(x, y);
	}
}
