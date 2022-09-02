#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H
#include <opencv2/core/core.hpp>

#include <QWidget>

/* Works with 8 bit image
 * */
class ImageProcessor
{
    Q_PROPERTY(size_t pixInCut READ getNumPixInCut WRITE setNumPixInCut)
public:
    ImageProcessor();
    void importImage( const QImage &new_image);

    void erodeImage( const cv::Mat& image, int iterations );
    void erodeImage(const cv::Mat& cross_iamge);

    //Parameters
    void setNumPixInCut( size_t number = 16){ pixels_in_cut_total = number; }
    size_t getNumPixInCut(){ return pixels_in_cut_total; }

    // Convert Operations
    static QImage  mat2image( const cv::Mat &inMat );
    static cv::Mat image2mat( const QImage &inImage );

    // Getters
    const cv::Mat& eroded() const {return eroded_image;}
    const cv::Mat& original() const {return original_image;}
private:
    bool enoughPixelsInCut( const cv::Mat& image );
private:
    cv::Mat original_image;
    cv::Mat eroded_image;
    size_t pixels_in_cut_total;
};

#endif // IMAGEPROCESSOR_H
