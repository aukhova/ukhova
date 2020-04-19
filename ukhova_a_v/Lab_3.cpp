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

Mat Gistogram_Gray(Mat& gray)
{
	//разбиваем на каналы
	vector<Mat> bgr_planes_gray;
	split(gray, bgr_planes_gray);

	// Establish the number of bins
	int histSize = 256;

	// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 }; //the upper boundary is exclusive
	const float* histRange = { range };


	bool uniform = true, accumulate = false;
	Mat w_hist;
	// Compute the histograms:
	calcHist(&bgr_planes_gray[0], 1, 0, Mat(), w_hist, 1, &histSize, &histRange, uniform, accumulate);

	// Draw the histograms for B, G and R
	int hist_w = 512, hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	
	Mat histImageG(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));

	// Normalize the result to [ 0, histImage.rows ]
	normalize(w_hist, w_hist, 0, histImageG.rows, NORM_MINMAX, -1, Mat());

	// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImageG, Point(bin_w*(i - 1), hist_h - cvRound(w_hist.at<float>(i - 1))),
		Point(bin_w*(i), hist_h - cvRound(w_hist.at<float>(i))),
		Scalar(255, 255, 255), 2, 8, 0);
	}
	
	return histImageG;
}

int My_Otsu(Mat& image, int left_board, int right_board) 
{
	// Вычисление гистограммы.
	vector<int> histogram(256);
	for (int j = 0; j < image.cols; ++j) 
	{
		for (int i = 0; i < image.rows; ++i) 
		{
			if ((int)image.at<uchar>(i, j) >= left_board &&(int)image.at<uchar>(i, j) < right_board) 
			{
				++histogram[(int)image.at<uchar>(i, j)];
			}
		}
	}
	// Инициализация необходимых переменных.
	
	unsigned hist_sum = accumulate(histogram.begin(), histogram.end(), 0);
	double first_class_mean = 0;
	double second_class_mean = 0;
	double icv = 0;
	double max_icv = icv;
	double first_class_sum = 0;
	double first_class_not_mean = 0;
	double sum = 0.0;int j = 0;
	for (auto bin_value = histogram.begin(); bin_value != histogram.end(); ++bin_value) 
	{
		sum += (j * (*bin_value));++j;
	}
	int threshold = 0;
	// Применение критерия Оцу.
	for (int j = 0; j <= 255; ++j) 
	{
		first_class_sum += histogram[j];
		if (first_class_sum == 0) 
		{
			continue;
		}
		double second_class_sum = hist_sum - first_class_sum;
		if (second_class_sum == 0) 
		{
			break;
		}
		first_class_not_mean += (float)(j * histogram[j]);
		first_class_mean = first_class_not_mean / first_class_sum;
		second_class_mean = (sum - first_class_not_mean) / second_class_sum;
		icv = (float)first_class_sum * second_class_sum * (first_class_mean - second_class_mean) * (first_class_mean - second_class_mean); 
		if (icv > max_icv) 
		{
			max_icv = icv; 
			threshold = j; 
	}
	}
	// Вычисленный порог.
	return threshold;
}


Mat My_global_binarize(Mat& image, int threshold) 
{
	image.setTo(0, image <= threshold);
	image.setTo(255, image > threshold);
	return image; 
}


Mat Image_extension_back_reflect_image(Mat& image, int x, int y)
{
	Mat result(image.rows + y - 1, image.cols + x - 1, image.type(), Scalar(0));

	if ((image.rows * 2 < result.rows) || (image.cols * 2 < result.cols))
	{
		cout << "Very big values for height or width" << endl;
		exit(-2);
	}

	// Заполняем основную сцену нового изображения
	int i_original = 0;
	for (int i = round(y / 2); i < result.rows - round(y / 2); i++)
	{
		int j_original = 0;
		for (int j = round(x / 2); j < result.cols - round(x / 2); j++)
		{
			result.at<uint8_t>(i, j) = image.at<uint8_t>(i_original, j_original);
			j_original++;
		}
		i_original++;
	}

	// Заполняем края
	int i_value = result.rows - round(y / 2) - 1;
	for (int i = round(y / 2) - 1; i >= 0; i--)
	{
		for (int j = 0; j < result.cols; j++)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i_value, j);
		}
		i_value--;
	}
	i_value = round(y / 2);
	for (int i = result.rows - round(y / 2); i < result.rows; i++)
	{
		for (int j = 0; j < result.cols; j++)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i_value, j);
		}
		i_value++;
	}
	for (int i = 0; i < result.rows; i++)
	{
		int j_value = result.cols - round(x / 2) - 1;
		for (int j = round(x / 2) - 1; j >= 0; j--)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i, j_value);
			j_value--;
		}
	}
	for (int i = 0; i < result.rows; i++)
	{
		int j_value = round(x / 2);
		for (int j = result.cols - round(x / 2); j < result.cols; j++)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i, j_value);
			j_value++;
		}
	}

	return result;
}

// square_x - длина прямоугольного окна
// square_y - ширина прямоугольного окна
// k - заранее подобранный коэффициент, определяющий значимость стандартного отклонения
Mat Nibleck(const Mat& image, const int square_x, const int square_y, const double k)
{
	if ((square_x % 2 == 0) || (square_y % 2 == 0))
	{
		cout << "Bad values of window" << endl;
		exit(-1);
	}
	//расширить картинку 
	Mat mat = image.clone();
	Mat extension_image = Image_extension_back_reflect_image(mat, square_x, square_y);

	// Создаём матрицу со значениями порога для каждого пикселя
	
	double** step_values = new double*[mat.rows];
	cout << "matt.cols= " << mat.cols << endl;
	cout << "matt.rows= " << mat.rows << endl;
	cout << "extension_image.rows - round(square_y / 2)= " << extension_image.rows - round(square_y / 2) << endl;
	cout << "extension_image.cols - round(square_x / 2)= " << extension_image.cols - round(square_x / 2) << endl;

	for (int i = 0; i < mat.rows; i++)
	{
		step_values[i] = new double[mat.cols];
	}

	int count_of_anomalous_values = 0;
	int i_original = 0;
	for (int i = round(square_y / 2); i < extension_image.rows - round(square_y / 2); i++)
	{
		int j_original = 0;
		for (int j = round(square_x / 2); j < extension_image.cols - round(square_x / 2); j++)
		{
			int m = 0; // Среднее значение яркости пикселей в пределах данного окна

			for (int ii = i - (square_y - 1) / 2; ii <= i + (square_y - 1) / 2; ii++)
			{
				for (int jj = j - (square_x - 1) / 2; jj <= j + (square_x - 1) / 2; jj++)
				{
					m += extension_image.at<uint8_t>(ii, jj);
				}
			}

			m = round(m / (square_x * square_y));

			double S = 0; // Стандартное отклонение

			for (int ii = i - (square_y - 1) / 2; ii <= i + (square_y - 1) / 2; ii++)
			{
				for (int jj = j - (square_x - 1) / 2; jj <= j + (square_x - 1) / 2; jj++)
				{
					S += ((double)extension_image.at<uint8_t>(ii, jj) - m) * ((double)extension_image.at<uint8_t>(ii, jj) - m);
				}
			}

			S = sqrt(S / ((double)square_x * (double)square_y));
			
			step_values[i_original][j_original] = m + k * S;
			
			if ((step_values[i_original][j_original] < 0) || (step_values[i_original][j_original] > 255))
			{
				count_of_anomalous_values++;
			}
			j_original++;
		}
		i_original++;
	}

	Mat nibleck_img = mat.clone();

	// Процесс бинаризации
	for (int i = 0; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++)
		{
			
			if ((double)mat.at<uint8_t>(i, j) < step_values[i][j])
			{
				nibleck_img.at<uint8_t>(i, j) = (uint8_t)0;
				
			}
			else
			{
				nibleck_img.at<uint8_t>(i, j) = (uint8_t)255;
				
			}
		}
	}

	return nibleck_img;
}
Mat Local_binarization(const Mat& image)
{
	//метод Ниблека локальной бинаризации
	Mat nibleck_img = Nibleck(image, 17, 17, 0.2);
	return nibleck_img;
}

// Морфологические преобразования - эрозия и дилатация
Mat Erosion(const Mat& image, const int& erosion_element, const int& erosion_size)
{
	int erosion_type = 0;
	if (erosion_element == 0)
	{
		erosion_type = MORPH_RECT;
	}
	else if (erosion_element == 1)
	{
		erosion_type = MORPH_CROSS;
	}
	else if (erosion_element == 2)
	{
		erosion_type = MORPH_ELLIPSE;
	}

	Mat element = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	Mat result;

	erode(image, result, element);
	return result;
}
Mat Dilatation(const Mat& image, const int& dilatation_element, const int& dilatation_size)
{
	int dilatation_type = 0;
	if (dilatation_element == 0)
	{
		dilatation_type = MORPH_RECT;
	}
	else if (dilatation_element == 1)
	{
		dilatation_type = MORPH_CROSS;
	}
	else if (dilatation_element == 2)
	{
		dilatation_type = MORPH_ELLIPSE;
	}

	Mat element = getStructuringElement(dilatation_type, Size(2 * dilatation_size + 1, 2 * dilatation_size + 1), Point(dilatation_size, dilatation_size));
	Mat result;

	dilate(image, result, element);

	return result;
}

Mat Alpha_blending(const Mat& image1, const Mat& image2)
{
	Mat image1_copy = image1.clone();
	Mat image2_copy = image2.clone();

	double alpha = 0.5;
	double beta = 1 - alpha;

	Mat result;
	addWeighted(image1_copy, alpha, image2_copy, beta, 0.0, result);

	return result;
}



int main()
{
	//оригинал
	//Mat img = imread("C:/Users/Master/Desktop/ukhova_a_v/build4/pencilsBW.jpg", CV_8UC1);
	//Mat img2 = imread("C:/Users/Master/Desktop/ukhova_a_v/12345.jpg");
	Mat img = imread("C:/Users/Master/Desktop/ukhova_a_v/12345BW.jpg", CV_8UC1);
	//Mat img = imread("C:/Users/Master/Desktop/ukhova_a_v/mirror.jpg", CV_8UC1);
	//imshow("original", img);

	//гистограмма
    Mat histImageG;

	//создание гистограммы
	histImageG = Gistogram_Gray(img);

	//размеры картинки для вывода
	int height = img.rows;
	int width = img.cols + 10 + histImageG.cols;

	//картинка для вывода
	Mat image_orig = Mat(height, width, img.type());

	Rect imageROI = Rect(0, 0, img.cols, img.rows);
	//добавляем оригинальную картинку
	img.copyTo(image_orig(imageROI));

	Rect imageROIH = Rect(0, 0, histImageG.cols, histImageG.rows);
	imageROIH.x = img.cols+10;
	imageROIH.y = 0;

	//добавляем гистограмму
	histImageG.copyTo(image_orig(imageROIH));
	imshow("image_orig", image_orig);

	//////////2 часть
	Mat func_grafic(Mat::zeros(257, 257, CV_8UC1)); //окно для вывода графика
	int func_mass[256]; //массив для значений функции
	for (int i = 0; i < 256; i++) //обнуляем
	{
		func_mass[i] = 0;
	}
	int stlb = 0;

	for (int i = 0; i < 256; i++) //забиваем массив значениями функции
	{
		if (i <= 128)
		{
			func_mass[i] = i + ((abs(i - 128)) / 2);
		}
		else
		{
			func_mass[i] = i - ((abs(i - 128)) / 2);
		}
	}

	for (int j_stolb = 0; j_stolb < 256; j_stolb++) // отрисовываем графиик
	{
		func_grafic.at<uchar>((256 - func_mass[j_stolb]), j_stolb) = 255;
	}

	//////////////////3 часть
	Mat img_func = img.clone(); //задаем картинку

	for (int i_strok = 0; i_strok < img_func.rows; i_strok++) //применяем look up table 
		for (int j_stolb = 0; j_stolb < img_func.cols; j_stolb++)
		{
			img_func.at<uchar>(i_strok, j_stolb) = func_mass[img_func.at<uchar>(i_strok, j_stolb)];
		}

	Mat histImageG_func;

	histImageG_func = Gistogram_Gray(img_func);

	height = img_func.rows;
	width = img_func.cols + 10 + histImageG_func.cols;

	Mat image_func = Mat(height, width, img_func.type());

	Rect imageROI2 = Rect(0, 0, img_func.cols, img_func.rows);
	img_func.copyTo(image_func(imageROI2));

	Rect imageROIH2 = Rect(0, 0, histImageG_func.cols, histImageG_func.rows);
	imageROIH2.x = img_func.cols + 10;
	imageROIH2.y = 0;
	histImageG_func.copyTo(image_func(imageROIH2));

	Rect imageROIH3 = Rect(0, 0, func_grafic.cols, func_grafic.rows);
	imageROIH3.x = img_func.cols + 10;
	imageROIH3.y = histImageG_func.rows+10;
	func_grafic.copyTo(image_func(imageROIH3));
	imshow("image_func", image_func);
	
	//часть 5

	//с функциями библиотеки
	//Mat img_Otsu;
	//threshold(вход, выход, порог, максимально значение, тип бинаризации(?));
	//double otsu=threshold(img, img_Otsu, 100,255, THRESH_OTSU);
	//THRESH_BINARY
	//otsu=125 автоматически

	Mat my_img_Otsu = img.clone();
	int my_otsu_ = My_Otsu(img, 0, 255);
	cout << "otsu_ = " << my_otsu_ << endl;

	my_img_Otsu = My_global_binarize(my_img_Otsu, my_otsu_);

	imshow("my_img_Otsu", my_img_Otsu);

	//часть 4
	Mat CLAHE_img1 = img.clone();
	Ptr<CLAHE> clahe1 = createCLAHE();
	//clahe1->setClipLimit(4);
	clahe1->setTilesGridSize(Size(1, 1));
	Mat dst1;
	clahe1->apply(CLAHE_img1, dst1);

	Mat histImageCLAHE_img1= Gistogram_Gray(dst1);
	height = dst1.rows;
	width = dst1.cols + 10 + histImageCLAHE_img1.cols;

	//картинка для вывода
	Mat image_CLAHE_img1 = Mat(height, width, dst1.type());

	Rect imageROI7 = Rect(0, 0, dst1.cols, dst1.rows);
	//добавляем оригинальную картинку
	dst1.copyTo(image_CLAHE_img1(imageROI7));

	Rect imageROIH7 = Rect(0, 0, histImageCLAHE_img1.cols, histImageCLAHE_img1.rows);
	imageROIH7.x = dst1.cols + 10;
	imageROIH7.y = 0;
	//добавляем гистограмму
	histImageCLAHE_img1.copyTo(image_CLAHE_img1(imageROIH7));
	imshow("CLAHE image1", image_CLAHE_img1);


	Mat CLAHE_img2 = img.clone();
	Ptr<CLAHE> clahe2 = createCLAHE();
	clahe2->setClipLimit(4);
	Mat dst2;
	clahe2->apply(CLAHE_img2, dst2);
	//Устанавливает порог для ограничения контрастности
	//clahe2->setClipLimit(125);
	//imshow("CLAHE image2", dst2);
	Mat histImageCLAHE_img2 = Gistogram_Gray(dst2);

	//картинка для вывода
	Mat image_CLAHE_img2 = Mat(height, width, dst2.type());
	Rect imageROI8 = Rect(0, 0, dst2.cols, dst2.rows);
	//добавляем оригинальную картинку
	dst2.copyTo(image_CLAHE_img2(imageROI8));

	Rect imageROIH8 = Rect(0, 0, histImageCLAHE_img2.cols, histImageCLAHE_img2.rows);
	imageROIH8.x = dst2.cols + 10;
	imageROIH8.y = 0;
	//добавляем гистограмму
	histImageCLAHE_img2.copyTo(image_CLAHE_img2(imageROIH8));
	imshow("CLAHE image2", image_CLAHE_img2);
	
	//по умолчанию
	Mat CLAHE_img3 = img.clone();
	Ptr<CLAHE> clahe3 = createCLAHE();
	//clahe3->setClipLimit(4);
	//clahe2->setTilesGridSize(Size(4, 4));
	Mat dst3;
	clahe3->apply(CLAHE_img3, dst3);
	//clahe3->setClipLimit(125);

	Mat histImageCLAHE_img3 = Gistogram_Gray(dst3);

	//картинка для вывода
	Mat image_CLAHE_img3 = Mat(height, width, dst3.type());
	//imshow("image_orig", image_orig);


	Rect imageROI9 = Rect(0, 0, dst3.cols, dst3.rows);
	//добавляем оригинальную картинку
	dst3.copyTo(image_CLAHE_img3(imageROI9));

	Rect imageROIH9 = Rect(0, 0, histImageCLAHE_img3.cols, histImageCLAHE_img3.rows);
	imageROIH9.x = dst3.cols + 10;
	imageROIH9.y = 0;
	//добавляем гистограмму
	histImageCLAHE_img3.copyTo(image_CLAHE_img3(imageROIH9));
	imshow("CLAHE image3", image_CLAHE_img3);

	//часть 6
	Mat loc_bin_img = img.clone();
	loc_bin_img=Local_binarization(loc_bin_img);
	imshow("Local_binarization", loc_bin_img);


	// часть 7
	//Mat result_image;
	Mat median_img;
	//Mat bilateral_image;

	// Медианный фильтр 
	medianBlur(loc_bin_img, median_img, 5);
	imshow("Median filter", median_img);
	
	// Эрозия и дилатация
	Mat erosion_dilatation_img;
	Mat dilatation_erosion_img;

	erosion_dilatation_img = Erosion(loc_bin_img, 2, 1);
	erosion_dilatation_img = Dilatation(loc_bin_img, 2, 1);

	dilatation_erosion_img = Dilatation(loc_bin_img, 2, 1);
	dilatation_erosion_img = Erosion(loc_bin_img, 2, 1);

	//imshow("Binary bw_mat", binary_mat);
	imshow("Erosion and dilatation", erosion_dilatation_img);
	imshow("Dilatation and erosion", dilatation_erosion_img);
	
	//часть 8
	// Альфа-блендинг
	Mat alpha_median_img;
	Mat alpha_bilateral_img;
	Mat alpha_erosion_dilatation_img;
	Mat alpha_dilatation_erosion_img;
	alpha_median_img=Alpha_blending(img, median_img);
	imshow("Alpha_median_image", alpha_median_img);
	//Alpha_bilateral_image = Alpha_blending(img, bilateral_image);
	//imshow("Alpha_bilateral_image", Alpha_bilateral_image);
	alpha_erosion_dilatation_img = Alpha_blending(img, erosion_dilatation_img);
	imshow("Alpha_erosion_dilatation_img", alpha_erosion_dilatation_img);
	alpha_dilatation_erosion_img = Alpha_blending(img, dilatation_erosion_img);
	imshow("Alpha_dilatation_erosion_img", alpha_dilatation_erosion_img);
	
    waitKey(0);
	return 0;
}

