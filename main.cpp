#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "parameters.h"
#define WIDTH 853
#define HEIGHT 480
#define WIDTH2 640
#define HEIGHT2 640
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	Mat src = imread("D:\\Documents\\Images\\sample2.jpg");
	//Mat x_map(HEIGHT, WIDTH, CV_32FC1, &_x_map);
	//Mat y_map(HEIGHT, WIDTH, CV_32FC1, &_y_map);
	Mat x_map2(HEIGHT2, WIDTH2, CV_32FC1, &_x_map_2);
	Mat y_map2(HEIGHT2, WIDTH2, CV_32FC1, &_y_map_2);
	Mat result;
	remap(src, result, x_map2, y_map2, INTER_LINEAR);
	imwrite("D:\\Documents\\Images\\mapping2_2.jpg", result);
	

	return 0;
}

