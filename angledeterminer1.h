#pragma once

#include <opencv2/core/core.hpp>
#include <vector>
#include <tuple>
template<typename T>
double LstSqr(const std::vector<T>&, const std::vector<T>&);

class AngleDeterminer
{
public:
    AngleDeterminer(const cv::Mat&, const double default_angle);
    AngleDeterminer(const cv::Mat&);

    template<typename T>
    std::vector<double> scanRows(const cv::Mat);
    template<typename T>
    std::vector<double> scanCols(const cv::Mat);
    std::pair<double,double> result();
    cv::Mat resultImage();
    template<typename T>
    std::pair<std::vector<T>,std::vector<T>> appropriateValues(const std::vector<T>& values);
private:
    double result_angle_rows{NAN},result_angle_cols{NAN};
    cv::Mat result_image;
};
