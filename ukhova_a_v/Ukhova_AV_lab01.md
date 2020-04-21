\#\# Работа 1. Исследование гамма-коррекции

Автор: Полевой Д.В.

Исполнитель: Ухова А.В.

Дата: 21.04.20



\#\#\# Задание

1\. Сгенерировать серое тестовое изображение \$I\_1\$ в виде прямоугольника размером 768х60 пикселя с плавным изменение пикселей от черного к белому, одна градация серого занимает 3 пикселя по горизонтали.

2\. Применить к изображению \$I\_1\$ гамма-коррекцию с коэффициентом из интервала 2.2-2.4 и получить изображение \$G\_1\$.

3\. Сгенерировать серое тестовое изображение \$I\_2\$ в виде прямоугольника размером 768х60 пикселя со ступенчатым изменением яркости от черного к белому (от уровня 5 с шагом 10), одна градация серого занимает 30 пикселя по горизонтали.

4\. Применить к изображению \$I\_2\$ гамма-коррекцию с коэффициентом из интервала 2.2-2.4 и получить изображение \$G\_2\$.

5\. Показать визуализацию результатов в виде одного изображения



\#\#\# Результаты

![image-20200421213909651](C:\Users\Дмитрий\AppData\Roaming\Typora\typora-user-images\image-20200421213909651.png)

![image-20200421213933271](C:\Users\Дмитрий\AppData\Roaming\Typora\typora-user-images\image-20200421213933271.png)

![image-20200421213955687](C:\Users\Дмитрий\AppData\Roaming\Typora\typora-user-images\image-20200421213955687.png)

![image-20200421214010647](C:\Users\Дмитрий\AppData\Roaming\Typora\typora-user-images\image-20200421214010647.png)

Рис. 1. Результаты работы программы (сверху вниз \$I\_1\$, \$G\_1\$, \$I\_2\$, \$G\_2\$)



\#\#\# Текст программы

```c++
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
	imwrite("image_1.jpg", img);
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
	imwrite("image_2.jpg", img3);

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
	imwrite("image_3.jpg", img4);

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
	imwrite("image_4.jpg", img6);

	waitKey(0);
	return 0;
}
```


