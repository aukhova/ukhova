#include <opencv2/opencv.hpp> 

using namespace cv;


int main()
{

	Mat img(Mat::zeros(160, 768, CV_8UC1));

	int fontFace(FONT_HERSHEY_PLAIN);
	double fontScale(2.0);
	int thickness(3);
	Point textOrg(50, 50);
	int pix = 0;

	for (int irow = 0; irow < 80; irow++)
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
	Mat img2(Mat::zeros(160, 768, CV_8UC1));
	img.convertTo(img2, CV_32FC1, 1.0 / 255);

	for (int irow = 80; irow < 160; irow++)
		for (int jcol = 0; jcol < img2.cols; jcol++)

		{
			img2.at<float_t>(irow, jcol) = pow(img2.at<float_t>(irow - 80, jcol), 2.2);
		}
	img2.convertTo(img, CV_8UC1, 255);
	imshow("", img);

	waitKey(0);
	return 0;
}