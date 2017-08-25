#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

#include "Mode.h"
#include "Videolog.h"

//#define DEBUG_ARMOR true
const int BLUE = 0;
const int RED = 1;
const int OBJECTCOLOR = 0;

class CArmorDetection
{
public:

	int judgeColor(Mat rect);
	void DrawRotatedRect(Mat & mImg, RotatedRect cRotatedRect, int iDrawStrength);
	int FindArmor(Mat mImg,Point& res);
};
