#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat Difference(Mat& img_1, Mat& img_2)
{
	int k = 20;
	Mat RGB_ = abs(img_1 - img_2)*k ;

	vector<Mat> channel_1;
	split(img_1, channel_1);

	vector<Mat> channel_2;
	split(img_2, channel_2);

	Mat B_ = abs(channel_2[0] - channel_1[0]) * k;
	Mat G_ = abs(channel_2[1] - channel_1[1]) * k;
	Mat R_ = abs(channel_2[2] - channel_1[2]) * k;

	Mat B_to_RGB;
	Mat G_to_RGB;
	Mat R_to_RGB;
	cvtColor(B_, B_to_RGB, COLOR_GRAY2BGR);
	cvtColor(G_, G_to_RGB, COLOR_GRAY2BGR);
	cvtColor(R_, R_to_RGB, COLOR_GRAY2BGR);

	Mat result = Mat(img_1.rows, img_1.cols * 4+30, img_1.type());
	Rect ImageROI = Rect(0, 0, img_1.cols, img_1.rows);

	ImageROI.x = 0;
	ImageROI.y = 0;
	RGB_.copyTo(result(ImageROI));

	ImageROI.x = img_1.cols+10;
	ImageROI.y = 0;
	B_to_RGB.copyTo(result(ImageROI));

	ImageROI.x = img_1.cols * 2+20;
	ImageROI.y = 0;
	G_to_RGB.copyTo(result(ImageROI));

	ImageROI.x = img_1.cols * 3+30;
	ImageROI.y = 0;
	R_to_RGB.copyTo(result(ImageROI));

	
	return result;
}

int main()
{
    Mat img = imread("../../../testdata/pencils__photo.jpg");
	imshow("pencils__photo.jpg", img);

	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);

	vector<int> compression_params;
	compression_params.push_back(IMWRITE_JPEG_QUALITY);
	compression_params.push_back(90);

    imwrite("../../../prj.lab/lab02/pencils_90.jpg", img, compression_params);

	compression_params.pop_back();
	compression_params.push_back(60);
    imwrite("../../../prj.lab/lab02/pencils_60.jpg", img, compression_params);

	Mat img_90 = imread("pencils_90.jpg");
	Mat img_60 = imread("pencils_60.jpg");

	Mat result=Difference(img_90, img_60);
	imshow("result.jpg", result);
    imwrite("../../../prj.lab/lab02/result.jpg", result);

	waitKey(0);
	return 0;
}
