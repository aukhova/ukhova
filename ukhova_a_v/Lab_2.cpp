#include <stdio.h> 
#include <opencv2/opencv.hpp> 
#include <iostream>
#include <ostream>

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;


Mat Show_gistogram(Mat& img, Mat& gray)
{
	//разбиваем на каналы
	vector<Mat> bgr_planes;
	split(img, bgr_planes);

	vector<Mat> bgr_planes_gray;
	split(gray, bgr_planes_gray);


	// Establish the number of bins
	int histSize = 256;

	// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 }; //the upper boundary is exclusive
	const float* histRange = { range };


	bool uniform = true, accumulate = false;
	Mat b_hist, g_hist, r_hist, w_hist;
	// Compute the histograms:
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes_gray[0], 1, 0, Mat(), w_hist, 1, &histSize, &histRange, uniform, accumulate);

	// Draw the histograms for B, G and R
	int hist_w = 512, hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
	

	// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(w_hist, w_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(w_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(w_hist.at<float>(i))),
			Scalar(255, 255, 255), 2, 8, 0);
	}
	
	return histImage;
}

int main()
{	//оригинал
	Mat img = imread("C:/Users/Master/Desktop/ukhova_a_v/pencils_orig.jpg");
	//imshow("original", img);

	// Конвертироем RGB изображение в оттенки серого
	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);
	//imshow("original gray", gray);


	//качество 90
	vector<int> compression_params;
	compression_params.push_back(IMWRITE_JPEG_QUALITY);
	compression_params.push_back(90);
	compression_params.push_back(0);

	imwrite("pencils_90.jpg", img, compression_params);
	Mat img90 = imread("C:/Users/Master/Desktop/ukhova_a_v/build4/pencils_90.jpg");
	//imshow("90", img90);

	// Конвертироем RGB изображение в оттенки серого
	Mat gray90;
	cvtColor(img90, gray90, CV_BGR2GRAY);
	//imshow("90 gray", gray90);

	//качество 30
	vector<int> compression_params2;
	compression_params2.push_back(IMWRITE_JPEG_QUALITY);
	compression_params2.push_back(30);
	compression_params2.push_back(0);

	imwrite("pencils_30.jpg", img, compression_params2);
	Mat img30 = imread("C:/Users/Master/Desktop/ukhova_a_v/build4/pencils_30.jpg");
	//imshow("30", img30);

	// Конвертироем RGB изображение в оттенки серого
	Mat gray30;
	cvtColor(img30, gray30, CV_BGR2GRAY);
	//imshow("30 gray", gray30);


    Mat histImage,histImage90, histImage30;

	histImage=Show_gistogram(img,gray);
	//imshow("histImage original", histImage);


	histImage90 = Show_gistogram(img90, gray90);
	//imshow("histImage 90", histImage90);


	histImage30 = Show_gistogram(img30, gray30);
	//imshow("histImage 30", histImage30);

	int height = img.rows;
	int width = img.cols + 10 + histImage.cols;

	Mat image_orig = Mat(height, width, img.type());

	Rect imageROI = Rect(0, 0, img.cols, img.rows);
	img.copyTo(image_orig(imageROI));

	Rect imageROIH = Rect(0, 0, histImage.cols, histImage.rows);
	
	imageROIH.x = img.cols+10;
	imageROIH.y = 0;
	histImage.copyTo(image_orig(imageROIH));
	imshow("image_orig", image_orig);


	Mat image_90 = Mat(height, width, img90.type());

	imageROIH.x = 0;
	imageROIH.y = 0;
	img90.copyTo(image_90(imageROI));

	imageROIH.x = img90.cols + 10;
	imageROIH.y = 0;
	histImage90.copyTo(image_90(imageROIH));
	imshow("image_90", image_90);


	Mat image_30 = Mat(height, width, img30.type());

	imageROIH.x = 0;
	imageROIH.y = 0;
	img30.copyTo(image_30(imageROI));

	imageROIH.x = img30.cols + 10;
	imageROIH.y = 0;
	histImage30.copyTo(image_30(imageROIH));
	imshow("image_30", image_30);


    waitKey(0);

	return 0;
}

