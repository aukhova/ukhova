
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/xfeatures2d.hpp>


#include <opencv2/opencv.hpp>

#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <ctime>
#include <filesystem>
#include <iostream>

using namespace cv;

using namespace std;



vector<string> Get_paths(const LPSTR& cPath, const string& extention)
{
    WIN32_FIND_DATA fd;
    HANDLE hFile;
    short len = lstrlen(cPath);
    lstrcat(cPath, "*");
    hFile = FindFirstFile(cPath, &fd);
    cPath[len] = 0;

    vector<string> paths;

    do {
        if (fd.cFileName[0] == '.')continue;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            lstrcat(cPath, fd.cFileName);
            lstrcat(cPath, "\\");
            Get_paths(cPath, extention);
            cPath[len] = 0;
        }
        else
        {
            char* tmp = strrchr(fd.cFileName, '.');
            if (tmp && !strcmp(tmp, extention.c_str()))
            {
                paths.push_back(fd.cFileName);
            }
        }
    } while (FindNextFile(hFile, &fd));
    FindClose(hFile);

    //cout << "Searched files" << endl;
    /*for (int i = 0; i < paths.size(); i++)
    {
        cout << paths[i] << endl;
    }
    cout << endl;*/
    return paths;
}


bool Distance(vector< Point2f > sceneCorners, float min)
{
    bool size = true;
    vector<float> my_vector(4);
    for (int i = 0; i < 3; i++)
    {
        my_vector[i] = sqrt((sceneCorners[i].x - sceneCorners[i + 1].x) * (sceneCorners[i].x - sceneCorners[i + 1].x) + (sceneCorners[i].y - sceneCorners[i + 1].y) * (sceneCorners[i].y - sceneCorners[i + 1].y));
    }
    my_vector[3] = sqrt((sceneCorners[3].x - sceneCorners[0].x) * (sceneCorners[3].x - sceneCorners[0].x) + (sceneCorners[3].y - sceneCorners[0].y) * (sceneCorners[3].y - sceneCorners[0].y));

    for (int i = 0; i < 4; i++)
    {
        if (my_vector[i] < min)
            size = false;
    }
        
    return size;
 }

bool Quad_convexity(const vector<Point2f>& points)
{
    float* helper_mas = new float[4];
    vector<Point2f> my_vector(4);

    for (int i = 0; i < 3; i++)
    {
        my_vector[i].x = points[i + 1].x - points[i].x;
        my_vector[i].y = points[i + 1].y - points[i].y;
    }

    my_vector[3].x = points[0].x - points[3].x;
    my_vector[3].y = points[0].y - points[3].y;

    for (int i = 0; i < 3; i++)
    {
        helper_mas[i] = my_vector[i].x * my_vector[i + 1].y - my_vector[i].y * my_vector[i + 1].x;
    }

    helper_mas[3] = my_vector[3].x * my_vector[0].y - my_vector[3].y * my_vector[0].x;

    for (int i = 0; i < 4; i++)
    {
        if (helper_mas[i] < 0)
        {
            //cout << "Not" << endl;
            return false;
        }
    }

    //cout << "Yes" << endl;
    return true;
}


int main( int argc, char** argv ) {

    /*if (argc != 3) {
        printf("Usage: surf.exe  image_sign image_scene\n");
        exit(1);
    }*/

    char images_Path[MAX_PATH] = "F:\\cur\\test04\\"; //E:\\testing\\test\\"; // Путь к папке с изображениями

    string image_extention = ".tif";
    vector<string> images_paths = Get_paths(images_Path, image_extention); // Хранятся пути к искажённым изображениям
    int n=images_paths.size();

    for (int j = 0; j < n; j++)
    {
        String objFileName = "F:\\cur\\samples\\01_alb_id.tif"; //argv[1];       // "F:\\projs\\Aleksandra\\opencv\\itnotesblog_ru\\step01\\Surf\\imgs\\one_sign.png";
        String sceneFileName = images_Path + images_paths[j];//argv[2];      // ]"F:\\projs\\Aleksandra\\opencv\\itnotesblog_ru\\step01\\Surf\\imgs\\scene.png";

        String str_i = to_string(j);
        //Mat objImage = imread( objFileName.toStdString(), CV_LOAD_IMAGE_GRAYSCALE );
        //Mat sceneImage = imread(sceneFileName.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);

        Mat objImage = imread(objFileName, CV_LOAD_IMAGE_GRAYSCALE);
        Mat sceneImage = imread(sceneFileName, CV_LOAD_IMAGE_GRAYSCALE);

        if (!objImage.data || !sceneImage.data) {
            return -3;
        }

        // Шаг 1: Найдем ключевые точки с помощью SURF-детектора
        static const int MIN_HESSIAN = 800;

        Ptr< xfeatures2d::SurfFeatureDetector > detector = xfeatures2d::SURF::create(MIN_HESSIAN);

        std::vector< KeyPoint > objKeypoints, sceneKeypoints;

        detector->detect(objImage, objKeypoints);
        detector->detect(sceneImage, sceneKeypoints);

        // Шаг 2: Высчитаем описатели или дескрипторы (векторы характерстик)
        Ptr< xfeatures2d::SurfDescriptorExtractor > extractor = xfeatures2d::SurfDescriptorExtractor::create();

        Mat objDescriptors, sceneDescriptors;

        extractor->compute(objImage, objKeypoints, objDescriptors);
        extractor->compute(sceneImage, sceneKeypoints, sceneDescriptors);

        // Шаг 3: Сопоставим векторы дескрипторов с помощью FLANN
        FlannBasedMatcher matcher;
        std::vector< DMatch > matches;
        matcher.match(objDescriptors, sceneDescriptors, matches);

        double max_dist = 0;
        double min_dist = 10;

        // Найдем максимальное и минимальное расстояние между ключевыми точками
        for (int i = 0; i < objDescriptors.rows; i++) {
            double dist = matches[i].distance;
            if (dist < min_dist) {
                min_dist = dist;
            }
            if (dist > max_dist) {
                max_dist = dist;
            }
        }

        // Нарисуем только "хорошие" совпадения (т.е. те, для которых расстояние меньше, чем 2*min_dist)
        std::vector< DMatch > goodMatches;

        for (int i = 0; i < objDescriptors.rows; i++) {
            if (matches[i].distance < 2*min_dist) {
                goodMatches.push_back(matches[i]);
            }
        }

        Mat imgMatches;
        drawMatches(objImage, objKeypoints, sceneImage, sceneKeypoints,
            goodMatches, imgMatches, Scalar::all(-1), Scalar::all(-1),
            std::vector< char >(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

        // Найдем объект на сцене
        std::vector< Point2f > obj;
        std::vector< Point2f > scene;

        for (size_t i = 0; i < goodMatches.size(); i++) {
            // Отбираем ключевые точки из хороших совпадений
            obj.push_back(objKeypoints[goodMatches[i].queryIdx].pt);
            scene.push_back(sceneKeypoints[goodMatches[i].trainIdx].pt);
        }

        //if ((obj.size() < 4) || (scene.size() < 4))
        //{
        //    //cout << "Quad not convexity" << endl;
        //    std::string resultFileName = "C:\\Users\\Master\\Desktop\\results\\bad\\points\\" + images_paths[j] + ".png";
        //    cv::imwrite(resultFileName, imgMatches);
        //    continue;
        //}


        Mat H = findHomography(obj, scene, CV_RANSAC);

        if ((H.rows == 0) || (H.cols == 0))
        {
            //std::string resultFileName = "C:\\Users\\Master\\Desktop\\results\\bad\\points\\" + images_paths[j] + ".png";
            std::string resultFileName = "F:\\cur\\bad\\" + images_paths[j] + ".png";
            cv::imwrite(resultFileName, imgMatches);
            continue;
        }

        // Занесем в вектор углы искомого объекта
        std::vector< Point2f > objCorners(4);
        objCorners[0] = cvPoint(0, 0);
        objCorners[1] = cvPoint(objImage.cols, 0);
        objCorners[2] = cvPoint(objImage.cols, objImage.rows);
        objCorners[3] = cvPoint(0, objImage.rows);

        

        std::vector< Point2f > sceneCorners(4);
        perspectiveTransform(objCorners, sceneCorners, H);

        // Нарисуем линии между углами (отображение искоромого объекта на сцене)
        line(imgMatches, sceneCorners[0] + Point2f(objImage.cols, 0), sceneCorners[1] + Point2f(objImage.cols, 0), Scalar(0, 255, 0), 34);
        line(imgMatches, sceneCorners[1] + Point2f(objImage.cols, 0), sceneCorners[2] + Point2f(objImage.cols, 0), Scalar(0, 255, 0), 34);
        line(imgMatches, sceneCorners[2] + Point2f(objImage.cols, 0), sceneCorners[3] + Point2f(objImage.cols, 0), Scalar(0, 255, 0), 34);
        line(imgMatches, sceneCorners[3] + Point2f(objImage.cols, 0), sceneCorners[0] + Point2f(objImage.cols, 0), Scalar(0, 255, 0), 34);


        cout << "image_"+ images_paths[j] << endl;
        cout << sceneCorners[0] << endl;
        cout << sceneCorners[1] << endl;
        cout << sceneCorners[2] << endl;
        cout << sceneCorners[3] << endl;
        cout << "..............." << endl;

        // Проверка на выпуклость
        if (!Quad_convexity(sceneCorners))
        {
            cout << "Quad not convexity" << endl;
            std::string resultFileName = "F:\\cur\\bad\\" + images_paths[j] + ".png";
            cv::imwrite(resultFileName, imgMatches);
            cout << "---------" << endl;
            continue;
        }
        // Покажем найденные совпадения
        //imshow("Good Matches & Object detection", imgMatches);

        // сохраним в файл.
        //std::string resultFileName = "C:\\Users\\Master\\Desktop\\res2\\result"+ str_i+".png";;
        //cv::imwrite(resultFileName, imgMatches);

        float min = 300;

        if (Distance(sceneCorners, min))
        {
            //std::string resultFileName = "C:\\Users\\Master\\Desktop\\results\\good\\" + images_paths[j] + ".png";
            std::string resultFileName = "F:\\cur\\good\\" + images_paths[j] + ".png";
            cv::imwrite(resultFileName, imgMatches);
        }
        else
        {
            //std::string resultFileName = "C:\\Users\\Master\\Desktop\\results\\bad\\" + images_paths[j] + ".png";;
            std::string resultFileName = "F:\\cur\\bad\\" + images_paths[j] + ".png";
            cv::imwrite(resultFileName, imgMatches);
        }
    }

    //проверка
    char images_Path_good[MAX_PATH] = "F:\\cur\\good\\"; // Путь к папке с изображениями
    char images_Path_bad[MAX_PATH] = "F:\\cur\\bad\\"; // Путь к папке с изображениями

    cout << "---------------------" << endl;
    string image_extention_ = ".png";
    vector<string> images_paths_good = Get_paths(images_Path_good, image_extention_); // Хранятся пути к искажённым изображениям
    vector<string> images_paths_bad = Get_paths(images_Path_bad, image_extention_); // Хранятся пути к искажённым изображениям
    int n_good = images_paths_good.size();
    int n_bad = images_paths_bad.size();
    cout << "---------------------" << endl;

    string number = "01";
    int good_good = 0;//правильный объект определен к правильным
    int good_bad = 0;//неправильный объект определен к правильным
    int bad_bad = 0;//неправильный объект определен к неправильным
    int bad_good = 0;//правильный объект определен к неправильным

    int img01 = 0;
    int img04 = 0;
    string number04 = "04";
    int img07 = 0;
    string number07 = "07";
    int img10 = 0;
    string number10 = "10";
    int img21 = 0;
    string number21 = "21";
    int img28 = 0;
    string number28 = "28";
    int img44 = 0;
    string number44 = "44";

    cout << "---------------------" << endl;
    string ch = "";
    for (int j = 0; j < n_good; j++)
    {
        if ((images_paths_good[j][2] == number[0])&&(images_paths_good[j][3] == number[1]))
        {
            good_good++;
            img01++;
        }
        else
        {
            good_bad++;
            if ((images_paths_good[j][2] == number04[0]) && (images_paths_good[j][3] == number04[1]))
            {
                img04++;
            }
            if ((images_paths_good[j][2] == number07[0]) && (images_paths_good[j][3] == number07[1]))
                img07++;
            if ((images_paths_good[j][2] == number10[0]) && (images_paths_good[j][3] == number10[1]))
                img10++;
            if ((images_paths_good[j][2] == number21[0]) && (images_paths_good[j][3] == number21[1]))
                img21++;
            if ((images_paths_good[j][2] == number28[0]) && (images_paths_good[j][3] == number28[1]))
                img28++;
            if ((images_paths_good[j][2] == number44[0]) && (images_paths_good[j][3] == number44[1]))
                img44++;
        }
    }

    for (int j = 0; j < n_bad; j++)
    {
        if ((images_paths_bad[j][2] == number[0]) && (images_paths_bad[j][3] == number[1]))
        {
            bad_good++;
            img01++;
        }
        else
        {
            bad_bad++;
            if ((images_paths_bad[j][2] == number04[0]) && (images_paths_bad[j][3] == number04[1]))
                img04++;
            if ((images_paths_bad[j][2] == number07[0]) && (images_paths_bad[j][3] == number07[1]))
                img07++;
            if ((images_paths_bad[j][2] == number10[0]) && (images_paths_bad[j][3] == number10[1]))
                img10++;
            if ((images_paths_bad[j][2] == number21[0]) && (images_paths_bad[j][3] == number21[1]))
                img21++;
            if ((images_paths_bad[j][2] == number28[0]) && (images_paths_bad[j][3] == number28[1]))
                img28++;
            if ((images_paths_bad[j][2] == number44[0]) && (images_paths_bad[j][3] == number44[1]))
                img44++;
        }
    }

    double accuracy = float(good_good+ bad_bad) / float(bad_bad+ good_good+ good_bad+ bad_good);
    float precision = float(good_good) / float(good_good + good_bad);
    float recall = float(good_good) / float(good_good + bad_good);

    cout << "..............." << endl;
    cout << "results:" << endl;
    cout << "good: "+to_string(n_good)+"...   TP:"+ to_string (good_good)+"...  FP:"+to_string(good_bad )<< endl;
    cout << "bad: " + to_string(n_bad) + "...   FN:" + to_string(bad_good) + "...  TN:" + to_string(bad_bad) << endl;

    //double accuracy = good_good / bad_bad;
    //float precision = good_good / (good_good + good_bad);
    //float recall = good_good / (good_good + bad_good);

    cout << "accuracy:" << accuracy << endl;
    cout << "precision:" << precision << endl;
    cout << "recall:" << recall << endl;

    cout << "---------------------" << endl;
    cout << "01:" + to_string(img01) << endl;
    cout << "04:" + to_string(img04) << endl;
    cout << "07:" + to_string(img07) << endl;
    cout << "10:" + to_string(img10) << endl;
    cout << "21:" + to_string(img21) << endl;
    cout << "28:" + to_string(img28) << endl;
    cout << "44:" + to_string(img44) << endl;
 

    waitKey( 0 );

    return 0;
}
