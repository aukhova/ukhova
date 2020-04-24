#include <stdio.h> 
#include <opencv2/opencv.hpp> 
#include <iostream>
#include <ostream>
#include <vector>
#include <numeric>
#include <string>
#include <functional>

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;


void Print_circles(Mat& image)
{
	//circle(Mat& картинка, Point центр, int радиус, const Scalar& цвет, int толщина (если отриц, то заполненный), int lineType = 8 тип границы, int shift = 0)
	circle(image, Point(50, 50), 25, Scalar(127), -1);
	circle(image, Point(150, 50), 25, Scalar(255), -1);
	circle(image, Point(250, 50), 25, Scalar(0), -1);
	circle(image, Point(50, 150), 25, Scalar(255), -1);
	circle(image, Point(150, 150), 25, Scalar(0), -1);
	circle(image, Point(250, 150), 25, Scalar(127), -1);
}


void Print_rectangle(Mat& image)
{
	//rectangle( Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness =1, int lineType =8, int shift =0 )
	rectangle(image, Point(0, 0), Point(100, 100), Scalar(0), -1);
	rectangle(image, Point(100, 0), Point(200, 100), Scalar(127), -1);
	rectangle(image, Point(200, 0), Point(300, 100), Scalar(255), -1);
	rectangle(image, Point(0, 100), Point(100, 200), Scalar(127), -1);
	rectangle(image, Point(100, 100), Point(200, 200), Scalar(255), -1);
	rectangle(image, Point(100, 200), Point(300, 200), Scalar(0), -1);
}

void Filter_1(Mat& image, Mat& result)
{
	Mat image_1 = image.clone();
	
	float kernelData_1[] = { 1.0f, 0.0f,
		0.0f, -1.0f };
	Mat kernel_1(2, 2, CV_32FC1, (float*)kernelData_1);

	filter2D(image, image_1, -1, kernel_1);
	
	result = image_1.clone();
}

void Filter_2(Mat& image, Mat& result)
{
	Mat image_2 = image.clone();
	
	float kernelData_2[] = { 0.0f, 1.0f,
	-1.0f, 0.0f };
	Mat kernel_2(2, 2, CV_32FC1, (float*)kernelData_2);

	filter2D(image, image_2, -1, kernel_2);

	result = image_2.clone();
}

void Filter_3(Mat& image, Mat& result)
{
	Mat image_1 = image.clone();
	Mat image_2 = image.clone();
	float kernelData_1[] = { 1.0f, 0.0f,
		0.0f, -1.0f };
	Mat kernel_1(2, 2, CV_32FC1, (float*)kernelData_1);

	float kernelData_2[] = { 0.0f, 1.0f,
		-1.0f, 0.0f };
	Mat kernel_2(2, 2, CV_32FC1, (float*)kernelData_2);

	filter2D(image, image_1, -1, kernel_1);
	filter2D(image, image_2, -1, kernel_2);

	for (int i = 0; i < 200; i++)
	{
		for (int j = 0; j < 300; j++)
		{
			result.at<float>(i, j) = (sqrt(image_1.at<float>(i, j) * image_1.at<float>(i, j) + image_2.at<float>(i, j) * image_2.at<float>(i, j)) );
		}
	}
}

int main()
{
	Mat image(200, 300, CV_8UC1, Scalar(0));

	Print_rectangle(image);
	Print_circles(image);
	imshow("image", image);
	imwrite("../../../prj.lab/lab04/lab04_image.jpg", image);

	Mat image_float;
	image.convertTo(image_float, CV_32FC1);

	Mat image_filter1(200, 300, CV_8UC1);
	Mat image_filter2(200, 300, CV_8UC1);
	Mat image_filter3(200, 300, CV_8UC1);


	Mat image1_float(200, 300, CV_32FC1);
	Mat image2_float(200, 300, CV_32FC1);
	Mat image3_float(200, 300, CV_32FC1);

	Filter_1(image_float, image1_float);
	image1_float.convertTo(image_filter1, CV_8UC1);
	imshow("Filter_1 ", image_filter1);
	imwrite("../../../prj.lab/lab04/lab04_image_filter1.jpg", image_filter1);

	Filter_2(image_float, image2_float);
	image2_float.convertTo(image_filter2, CV_8UC1);
	imshow("Filter_2", image_filter2);
	imwrite("../../../prj.lab/lab04/lab04_image_filter2.jpg", image_filter2);

	Filter_3(image_float, image3_float);
	image3_float.convertTo(image_filter3, CV_8UC1);
	imshow("Filter_3", image_filter3);
	imwrite("../../../prj.lab/lab04/lab04_image_filter3.jpg", image_filter3);

	waitKey(0);

	return 0;
}
