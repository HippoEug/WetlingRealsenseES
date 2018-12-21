#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "csvmanager.hpp"
#include "volumefunctions.hpp"

void VolumeFunctions::getPlaneBoundary() {
	// Declaration to read 921,600 points from CSV file
	std::ifstream importVerticesFile;
	Array2D<double, 1280, 720> vertices;
	char readCSVRemover; // Removes ',' and '\n' to import 921,600 points from CSV file

	// Declaration to manage plane boundary selection points
	std::ifstream planeBoundaryFile;

	std::string pbX, pbY;  // For getting (x, y) coordinate of plane boundary
	std::vector<int> pbXVector;
	std::vector<int> pbYVector;

	int noOfPointsPlaneBoundary = 0; // To find out number of points for plane average
	double totalPlaneHeight = 0; // To find out total height for plane average
	double avgPlaneHeight = 0; // To store plane average height

	// Function Start
	importVerticesFile.open("abc_Depth.csv");
	if (!importVerticesFile.is_open()) {
		std::cout << "Error in opening abc_Depth.csv" << std::endl;
	}

	planeBoundaryFile.open("planeBoundaryFile.csv");
	if (!planeBoundaryFile.is_open()) {
		std::cout << "Error in opening planeBoundaryFile.csv" << std::endl;
	}

	// To import 921,600 points from CSV file
	std::cout << "COPYING FROM abc_Depth.csv" << std::endl;
	for (int y{}; y < 720; ++y) {
		for (int x{}; x < 1280; ++x) {
			if (!(importVerticesFile >> vertices(x, y) >> std::noskipws >> readCSVRemover >> std::skipws) && !importVerticesFile.eof() && readCSVRemover != ',' && readCSVRemover != '\n') {
				std::cerr << "Format error at " << x + 1 << '/' << y + 1 << "\n\n";
			}
		}
	}
	std::cout << "COPYING FROM abc_Depth.csv [COMPLETED]" << std::endl;

	// To import planeBoundaryFile.csv
	std::cout << "COPYING FROM planeBoundaryFile.csv" << std::endl;
	while (planeBoundaryFile.good()) {
		std::getline(planeBoundaryFile, pbX, ','); // Gets X value and stores in pbX as string
		std::getline(planeBoundaryFile, pbY, '\n'); // Gets Y value and stores in pbY as string

		pbXVector.push_back(std::stoi(pbX)); // Stores pbX values as <int> vector
		pbYVector.push_back(std::stoi(pbY)); // Stores pbY values as <int> vector
		noOfPointsPlaneBoundary++;
	}
	std::cout << "COPYING FROM planeBoundaryFile.csv [COMPLETED]" << std::endl;
	std::cout << "\nnoOfPointsPlaneBoundary = " << noOfPointsPlaneBoundary << std::endl;

	// Function to get average plane height
	for (int count{}; count < noOfPointsPlaneBoundary; count++) {
		double temp = vertices(pbXVector.at(count) - 1, pbYVector.at(count) - 1);
		totalPlaneHeight += temp;
	}
	std::cout << "totalPlaneHeight = " << totalPlaneHeight << std::endl;
	avgPlaneHeight = totalPlaneHeight / noOfPointsPlaneBoundary;
	std::cout << "avgPlaneHeight = " << avgPlaneHeight << std::endl;

	importVerticesFile.close();
	planeBoundaryFile.close();
}

void VolumeFunctions::getWoundBoundary() {
	// Declaration to read 921,600 points from CSV file
	std::ifstream importVerticesFile;
	Array2D<double, 1280, 720> vertices;
	char readCSVRemover; // Removes ',' and '\n' to import 921,600 points from CSV file

	// Declaration to manage wound boundary selection points
	std::ifstream woundBoundaryFile;

	std::string wbX, wbY; // For getting (x, y) coordinate of wound boundary
	std::vector<int> wbXVector;
	std::vector<int> wbYVector;

	int noOfPointsWoundBoundary = 0; // To find out number of points created by user

	int noOfPointsWithinWoundBoundary = 0;
	double totalHeightInWoundBoundary = 0;

	int smallestX = 0, largestX = 0, smallestY = 0, largestY = 0;
	int leftMostX = 0, rightMostX = 0;
	int xLocation = 0, yLocation = 0;

	// Function Start
	importVerticesFile.open("abc_Depth.csv");
	if (!importVerticesFile.is_open()) {
		std::cout << "Error in opening abc_Depth.csv" << std::endl;
	}

	woundBoundaryFile.open("woundBoundaryFile.csv");
	if (!woundBoundaryFile.is_open()) {
		std::cout << "Error in opening woundBoundaryFile.csv" << std::endl;
	}

	// To import 921,600 points from CSV file
	std::cout << "\nCOPYING FROM abc_Depth.csv" << std::endl;
	for (int y{}; y < 720; ++y) {
		for (int x{}; x < 1280; ++x) {
			if (!(importVerticesFile >> vertices(x, y) >> std::noskipws >> readCSVRemover >> std::skipws) && !importVerticesFile.eof() && readCSVRemover != ',' && readCSVRemover != '\n') {
				std::cerr << "Format error at " << x + 1 << '/' << y + 1 << "\n\n";
			}
		}
	}
	std::cout << "COPYING FROM abc_Depth.csv [COMPLETED]" << std::endl;

	// To import planeBoundaryFile.csv
	std::cout << "COPYING FROM woundBoundaryFile.csv" << std::endl;
	while (woundBoundaryFile.good()) {
		std::getline(woundBoundaryFile, wbX, ',');
		std::getline(woundBoundaryFile, wbY, '\n');

		wbXVector.push_back(std::stoi(wbX));
		wbYVector.push_back(std::stoi(wbY));
		noOfPointsWoundBoundary++;
	}
	std::cout << "COPYING FROM woundBoundaryFile.csv [COMPLETED]" << std::endl;
	std::cout << "\nnoOfPointsWoundBoundary = " << noOfPointsWoundBoundary << std::endl;

	// To get smallest and largest (x, y) values
	for (int z = 0; z < noOfPointsWoundBoundary; z++) { // To get smallestX
		if (smallestX != 0) {
			if (wbXVector.at(z) <= smallestX) {
				smallestX = wbXVector.at(z);
			}
		}
		else {
			smallestX = wbXVector.at(0);
		}
	}

	for (int z = 0; z < noOfPointsWoundBoundary; z++) { // To get smallestY
		if (smallestY != 0) {
			if (wbYVector.at(z) <= smallestY) {
				smallestY = wbYVector.at(z);
			}
		}
		else {
			smallestY = wbYVector.at(0);
		}
	}

	for (int z = 0; z < noOfPointsWoundBoundary; z++) { // To get largestX
		if (largestX != 0) {
			if (wbXVector.at(z) >= largestX) {
				largestX = wbXVector.at(z);
			}
		}
		else {
			largestX = wbXVector.at(0);
		}
	}

	for (int z = 0; z < noOfPointsWoundBoundary; z++) { // To get largestY
		if (largestY != 0) {
			if (wbYVector.at(z) >= largestY) {
				largestY = wbYVector.at(z);
			}
		}
		else {
			largestY = wbYVector.at(0);
		}
	}

	std::cout << "\nSmallestX = " << smallestX << "\nLargest X = " << largestX << std::endl;
	std::cout << "SmallestY = " << smallestY << "\nLargest Y = " << largestY << std::endl << std::endl;

	// To get left most and right most x value for each horizontal line (y value) for summation
	for (int currentYCoordinate = smallestY; currentYCoordinate <= largestY; currentYCoordinate++) { // Test file has 60 different y values to go over {
		for (int count{}; count < noOfPointsWoundBoundary; count++) { // to go through all the data just to find the first value
			xLocation = wbXVector.at(count);
			yLocation = wbYVector.at(count);

			if (yLocation == currentYCoordinate) {
				// Check for leftmost and rightmost x value
				if (leftMostX != 0) { // Check for leftMostX
					if (xLocation <= leftMostX) {
						leftMostX = xLocation;
					}
				}
				else {
					leftMostX = xLocation;
				}

				if (rightMostX != 0) {
					if (xLocation >= rightMostX) {
						rightMostX = xLocation;
					}
				}
				else {
					rightMostX = xLocation;
				}
			}
		}
		 
		for (int xCount = leftMostX; xCount <= rightMostX; xCount++) {
			double tempVerticesValue = vertices(xCount - 1, currentYCoordinate - 1);
			totalHeightInWoundBoundary += tempVerticesValue;
			noOfPointsWithinWoundBoundary++;
		}

		leftMostX = 0;
		rightMostX = 0;
	}
	std::cout << "noOfPointsWoundBoundary = " << noOfPointsWoundBoundary << std::endl;
	std::cout << "noOfPointsWithinWoundBoundary = " << noOfPointsWithinWoundBoundary << std::endl;
	std::cout << "totalHeightInWoundBoundary = " << totalHeightInWoundBoundary << std::endl;

	importVerticesFile.close();
	woundBoundaryFile.close();
}
