#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;


int main()
{
	Mat img = imread("C:/Users/Master/Desktop/ukhova_a_v/cross_0256x0256.png", 1);

	Mat diffimage(Mat::zeros(512, 1024, CV_8UC3));


	Mat imgRed = imread("C:/Users/Master/Desktop/ukhova_a_v/cross_0256x0256.png", 1);
	imgRed = img & cv::Scalar(0, 0, 255);

	Mat imgGreen = imread("C:/Users/Master/Desktop/ukhova_a_v/cross_0256x0256.png", 1);
	imgGreen = img & cv::Scalar(0, 255, 0);

	Mat imgBlue = imread("C:/Users/Master/Desktop/ukhova_a_v/cross_0256x0256.png", 1);
	imgBlue = img & cv::Scalar(255, 0, 0);


	Mat channel[3];
	split(img, channel);


	Mat empty_image = Mat::zeros(img.rows, img.cols, CV_8UC1);
	Mat imgBlue_(img.rows, img.cols, CV_8UC3);
	Mat imgGreen_(img.rows, img.cols, CV_8UC3);
	Mat imgRed_(img.rows, img.cols, CV_8UC3);


	Mat in1[] = { channel[0], channel[0], channel[0] };
	int from_to1[] = { 0,0, 1,1, 2,2 };
	mixChannels(in1, 3, &imgBlue_, 1, from_to1, 3);

	Mat in2[] = { channel[1], channel[1], channel[1] };
	mixChannels(in2, 3, &imgGreen_, 1, from_to1, 3);

	Mat in3[] = { channel[2], channel[2], channel[2] };
	mixChannels(in3, 3, &imgRed_, 1, from_to1, 3);


	for (int i = 0; i < 256; i += 1)
		for (int j = 0; j < 256; j += 1)
		{

			//оригинал
			diffimage.at<cv::Vec3b>(i, j) = img.at<cv::Vec3b>(i, j);

			//верхние
			diffimage.at<cv::Vec3b>(i, j + 256) = imgBlue_.at<cv::Vec3b>(i, j);
			diffimage.at<cv::Vec3b>(i, j + 512) = imgGreen_.at<cv::Vec3b>(i, j);
			diffimage.at<cv::Vec3b>(i, j + 768) = imgRed_.at<cv::Vec3b>(i, j);

			//нижние
			diffimage.at<cv::Vec3b>(i + 256, j + 256) = imgBlue.at<cv::Vec3b>(i, j);
			diffimage.at<cv::Vec3b>(i + 256, j + 512) = imgGreen.at<cv::Vec3b>(i, j);
			diffimage.at<cv::Vec3b>(i + 256, j + 768) = imgRed.at<cv::Vec3b>(i, j);

		}
	imshow("Resault", diffimage);
	waitKey(0);
	return 0;
}