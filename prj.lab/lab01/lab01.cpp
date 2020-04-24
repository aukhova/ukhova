#include <opencv2/opencv.hpp> 

using namespace cv;


int main()
{

	Mat img(Mat::zeros(60, 768, CV_8UC1));

	int fontFace(FONT_HERSHEY_PLAIN);
	double fontScale(2.0);
	int thickness(3);
	Point textOrg(50, 50);
	int pix = 0;

	for (int irow = 0; irow < 60; irow++)
	{
		for (int jcol = 0; jcol < img.cols; jcol = jcol + 3)

		{
			img.at<uint8_t>(irow, jcol) = pix;
			img.at<uint8_t>(irow, jcol + 1) = pix;
			img.at<uint8_t>(irow, jcol + 2) = pix;

			pix++;
		}
		pix = 0;
	}
	imshow("img", img);
    imwrite("../../../prj.lab/lab01/lab01_image_1.png", img);
	Mat img2(Mat::zeros(60, 768, CV_8UC1));
	img.convertTo(img2, CV_32FC1, 1.0 / 255);

	for (int irow = 0; irow < 60; irow++)
		for (int jcol = 0; jcol < img2.cols; jcol++)

		{
			img2.at<float_t>(irow, jcol) = pow(img2.at<float_t>(irow, jcol), 2.2);
		}
	Mat img3;
	img2.convertTo(img3, CV_8UC1, 255);
	imshow("img2", img3);
    imwrite("../../../prj.lab/lab01/lab01_image_2.png", img3);

	Mat img4(Mat::zeros(60, 768, CV_8UC1));

	
	int pix2 = 5;

	for (int irow = 0; irow < 60; irow++)
	{
		for (int jcol = 0; jcol < img4.cols; jcol = jcol + 30)
		{
			for (int jfcol = 0; jfcol < 30; jfcol++)
				if (jfcol + jcol < img4.cols)
					img4.at<uint8_t>(irow, jcol + jfcol) = pix2;
			pix2 = pix2 + 10;
		}
		pix2 = 5;
	}
	imshow("img3", img4);
    imwrite("../../../prj.lab/lab01/lab01_image_3.png", img4);

	Mat img5(Mat::zeros(60, 768, CV_8UC1));
	img4.convertTo(img5, CV_32FC1, 1.0 / 255);

	for (int irow = 0; irow < 60; irow++)
		for (int jcol = 0; jcol < img5.cols; jcol++)
		{
			img5.at<float_t>(irow, jcol) = pow(img5.at<float_t>(irow, jcol), 2.2);
		}
	Mat img6;
	img5.convertTo(img6, CV_8UC1, 255);
	imshow("img4", img6);
    imwrite("../../../prj.lab/lab01/lab01_image_4.png", img6);

	waitKey(0);
	return 0;
}
