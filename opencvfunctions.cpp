#include <opencv2/opencv.hpp>
#include "opencvfunctions.hpp"

void selectCoordinates() {
	cv::Mat rgbImage;
	rgbImage = cv::imread("capture-Color.png"); // Open Image

	if (rgbImage.empty()) {
		std::cout << "Error: Image Not Found!" << std::endl << std::endl;
		cv::waitKey(3);
	}

	cv::namedWindow("Points Selection", CV_WINDOW_AUTOSIZE); // Fixed-size window, matching resolution of image
															 //cv::namedWindow("Points Selection", CV_WINDOW_NORMAL); // Allows resizing of window
	setMouseCallback("Points Selection", onMouse, &rgbImage); // Pass ptr to mat here
	cv::imshow("Points Selection", rgbImage);

	cv::waitKey(0); // Hold window open until user presses a key

	cv::destroyAllWindows();

}

static void onMouse(int event, int x, int y, int, void* imgptr) {
	using namespace cv;

	if (event != 1) { // draw only on left mouse click
		return;
	}

	Mat& img = (*(Mat*)imgptr); // cast first and dereference
	Point pt1 = Point(x, y);

	circle(img, pt1, 1, Scalar(0, 255, 0), 30, 8, 0); //RGB 0 255 0 = GREEN

	std::cout << "X: " << x << " Y: " << y << std::endl;

	imshow("Points Selection", img);
	waitKey(1);
}
