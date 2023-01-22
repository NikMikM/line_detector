#include "imageprocessor.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/matx.hpp>

ImageProcessor::ImageProcessor(): pixels_in_cut_total{16}
{

}

void ImageProcessor::importImage( const QImage &new_image )
{
    original_image = image2mat(new_image);
}

void ImageProcessor::erodeImage( const cv::Mat& image, int iterations )
{
    cv::Mat kernel = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT,cv::Size(3,3));
    cv::erode( image, eroded_image, kernel, cv::Point(-1,-1), iterations);
}

void ImageProcessor::erodeImage(const cv::Mat& cross_image)
{
    cv::Mat kernel = cv::getStructuringElement(cv::MorphShapes::MORPH_CROSS,cv::Size(3,3));
    cv::Mat original = cross_image.clone();
    cv::Mat result = cross_image.clone();
    int iterations = 0;
    while (enoughPixelsInCut(result) && (iterations < 10))
    {
        cv::erode(original, result, kernel);
        ++iterations;
        original = result.clone();
    }
    eroded_image = result;
}
bool ImageProcessor::enoughPixelsInCut(const cv::Mat &image)
{
    int h = (image.cols%2 == 0)? image.cols : image.cols - 1;
    int w = (image.rows%2 == 0)? image.rows : image.rows - 1;
    size_t pixels_in_cut = 0;
    int overall_intensity = 0;
    try {
        for (int i = 0; i < image.cols; i++) { // X scan
            for (int j = 0; j < image.rows; j++) { // Y scan
//            if ((j == i*w/h + w/2) || (j == -i*w/h +1.5*w) || (j == i*w/h - w/2) || (j == -i*w/h + w/2)){ // X(j) - down, Y(i) - right // Scan from centers of the edged
               if ((j == i*w/h +w/4) || (j == -i*w/h +1.25*w) || (j == i*w/h - w/4) || (j == -i*w/h + 3*w/4)){ // X(j) - down, Y(i) - right // Scan from centers of the edged

                cv::Vec3b intensity = image.at<cv::Vec3b>(j, i);
                overall_intensity = 0;
                for(int k = 0; k < image.channels(); k++) {
                    overall_intensity += intensity.val[k];
                }
                if (overall_intensity/image.channels() > 200) ++pixels_in_cut;
            }
            }
        }
    }  catch (std::exception& e) {
        qWarning() << "Errors while scanning image for erosion";
        return true;
    }
    qDebug() << "number of pixels in cut" << pixels_in_cut;
//    qDebug() << "pixels_in_cut_total" << pixels_in_cut_total;
    if (pixels_in_cut < pixels_in_cut_total) return false;
    return true;
}

cv::Mat ImageProcessor::image2mat( const QImage& inImage )
{
    switch ( inImage.format() )
    {
    // 8-bit, 4 channel
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    {
        cv::Mat  mat( inImage.height(), inImage.width(),
                      CV_8UC4,
                      const_cast<uchar*>(inImage.bits()),
                      static_cast<size_t>(inImage.bytesPerLine())
                      );

        return  mat.clone();
    }

    // 8-bit, 3 channel
    case QImage::Format_RGB32:
    case QImage::Format_RGB888:
    {

        QImage   swapped = inImage;

        if ( inImage.format() == QImage::Format_RGB32 )
        {
            swapped = swapped.convertToFormat( QImage::Format_RGB888 );
        }

        swapped = swapped.rgbSwapped();

        return cv::Mat( swapped.height(), swapped.width(),
                        CV_8UC3,
                        const_cast<uchar*>(swapped.bits()),
                        static_cast<size_t>(swapped.bytesPerLine())
                        ).clone();
    }

    // 8-bit, 1 channel
    case QImage::Format_Grayscale8:
    {
        cv::Mat  mat( inImage.height(), inImage.width(),
                      CV_8UC1,
                      const_cast<uchar*>(inImage.bits()),
                      static_cast<size_t>(inImage.bytesPerLine())
                      );

        return mat.clone();
    }

    default:
        qWarning() << "CVS::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
        break;
    }

    return cv::Mat();
}



QImage  ImageProcessor::mat2image( const cv::Mat& inMat )
{
    switch ( inMat.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_ARGB32 );

        return image;
    }

    // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_RGB888 );

        return image.rgbSwapped();
    }

    // 8-bit, 1 channel
    case CV_8UC1:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_Grayscale8 );//Format_Alpha8 and Format_Grayscale8 were added in Qt 5.5

        return image;
    }

    default:
        qWarning() << "CVS::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }

    return QImage();
}
