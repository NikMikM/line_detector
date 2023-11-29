#include "angledeterminer1.h"

#include <opencv2/imgproc.hpp>
//#include <opencv2/imgproc/hal/hal.hpp>

#include <numeric>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

#include <QDebug>
static int numba = 0;
AngleDeterminer::AngleDeterminer(const cv::Mat& image, const double default_angle)
{
    cv::Point center = cv::Point(image.cols/2,image.rows/2);
    cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, default_angle, 1.0);
    cv::Mat rotated_image;
    cv::warpAffine(image, rotated_image,rotation_matrix,image.size());

//    cv::Mat noise(image.rows,image.cols,rotated_image.type());
//    cv::randn(noise,0,10.0);
//    cv::add(rotated_image,noise,rotated_image);
//    std::random_device rd;  // a seed source for the random number engine
//    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
//    std::uniform_int_distribution<> distrib(0,0);
//    for(auto i = 0; i < rotated_image.rows; i++)
//    {
//        for(auto j = 0; j < rotated_image.cols;j++) {
//            auto num = distrib(gen);
//            if( rotated_image.at<uchar>(i,j) + num < 255)
//                rotated_image.at<uchar>(i,j) += num;

//        }
//    }

    std::vector<double> center_rows = scanRows<uchar>(rotated_image);
    std::vector<double> center_cols = scanCols<uchar>(rotated_image);

//    std::vector<double> angle_cols = scanCols<uchar>(image);
//    std::vector<double> rows_indexes(center_rows);
//    std::iota(rows_indexes.begin(), rows_indexes.end(),0);
    auto x_y_values= appropriateValues(center_rows);
    auto x_y_values2= appropriateValues(center_cols);

//    std::vector<double> out1;
    result_angle_rows = LstSqr<double>(x_y_values.first,x_y_values.second);
    result_angle_cols = LstSqr<double>(x_y_values2.second, x_y_values2.first);
    //    rotated_image.copyTo(result_image);
    result_image = rotated_image.clone();

//    qDebug() << numba++ <<"values from rows: " << result_angle;
//    qDebug() << "values from cols: " <<LstSqr<double>(x_y_values2.second,x_y_values2.first);


//    try{
//        cv::solve(angle_rows, rows_indexes, out1, cv::DECOMP_SVD);
//    } catch (cv::Exception& ex) {
//        std::cout << ex.what() << std::endl;
//    }
//    qDebug() << "Found values or value:";
//    for(auto elem : center_rows)
//    {
//        qDebug() << elem;
//    }
//    result_angle = angle_rows.at(0);
}
AngleDeterminer::AngleDeterminer(const cv::Mat& image)
{
    std::vector<double> center_rows = scanRows<uchar>(image);
    std::vector<double> center_cols = scanCols<uchar>(image);
    auto x_y_values= appropriateValues(center_rows);
    auto x_y_values2= appropriateValues(center_cols);
    result_angle_rows = LstSqr<double>(x_y_values.first,x_y_values.second);
    result_angle_cols = LstSqr<double>(x_y_values2.second, x_y_values2.first);
}

std::pair<double,double> AngleDeterminer::result()
{
    return {result_angle_rows, result_angle_cols};
}

cv::Mat AngleDeterminer::resultImage()
{
    return result_image.clone();
}

template<typename T>
std::vector<double> AngleDeterminer::scanRows(const cv::Mat image)
{
    std::vector<double> result(image.rows, 0);
    double summ1{0},summ2{0};
    for(size_t j = 0; j < image.rows; j++)
    {
        summ1 = 0;
        summ2 = 0;
        for(size_t i = 0; i < image.cols; i++)
        {
            summ1 += i*image.at<T>(i,j);
            summ2 += image.at<T>(i,j);
        }
        if(summ2 > 0)
        {
            result[j] = summ1 / summ2;
        }
    }
    return result;
}

template<typename T>
std::vector<double> AngleDeterminer::scanCols(const cv::Mat image)
{
    std::vector<double> result(image.cols, 0);
    double summ1{0},summ2{0};
    for(size_t i = 0; i < image.cols; i++)
    {
        summ1 = 0;
        summ2 = 0;
        for(size_t j = 0; j < image.rows; j++)
        {
            summ1 += j*image.at<T>(i,j);
            summ2 += image.at<T>(i,j);
        }
        if(summ2 > 0)
        {
            result[i] = summ1 / summ2;
        }
    }
    return result;
}

template<typename T>
double LstSqr(const std::vector<T> & x_coords, const std::vector<T> & y_coords)
{
    if ((x_coords.size() == 0) || (y_coords.size() == 0))
        return NAN;
    double x_mean = static_cast<double>(std::accumulate(x_coords.begin(), x_coords.end(), 0)) / x_coords.size();
    double y_mean = -static_cast<double>(std::accumulate(y_coords.begin(), y_coords.end(), 0)) / y_coords.size(); //Translated to decard system
    double top{}, bot{0};
    for(auto i = 0 ; i < x_coords.size(); i++)
    {
        top += (x_coords[i] - x_mean) * (-y_coords[i] - y_mean);//Translated to decard system
        bot += std::pow(x_coords[i] - x_mean,2);
    }
    double m = top / bot;
    return atan(m) * 180.0 / M_PI;
}

template<typename T>
std::pair<std::vector<T>,std::vector<T>> AngleDeterminer::appropriateValues(const std::vector<T> &values)
{
    auto threshold_input_value = 0.2 * (*std::max_element(values.begin(), values.end())); //value to be considered signal must be >20% of max
    auto comp = [threshold_input_value](const T& elem){return elem > threshold_input_value;};
    auto it1 = std::find_if(values.cbegin(), values.cend(), comp);
    auto it2 = std::find_if(values.crbegin(), values.crend(), comp);
    auto it3 = values.end() - std::distance(values.crbegin(),it2) - 1;
    auto first_num = std::distance(values.begin(),it1);
    std::vector<T> result(std::distance(it1,it3));
    std::iota(result.begin(), result.end(), first_num);
    return std::make_pair(result,std::vector<T>{it1,it3});
}

