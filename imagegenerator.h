#ifndef IMAGEGENERATOR_H
#define IMAGEGENERATOR_H

#include <QObject>
#include <QImage>
#include <QGenericMatrix>
#include <QGraphicsEffect>

// All member and parameter angles are degrees
class ImageGenerator
{
    Q_PROPERTY(size_t lineWidth READ get_line_width WRITE set_line_width CONSTANT )
public:
    ImageGenerator(double focal_length,double number_of_points, QSize img_size,double alpha1, double beta);
    ImageGenerator(double focal_length,double number_of_points, QSize img_size,double alpha1, double alpha2, double beta);
    ~ImageGenerator();
    void set_line_width( size_t width ){line_width = width;}
    size_t get_line_width() const {return line_width;}

    void set_rotation_matrix(double teta1, double teta2, double teta3);

    QGenericMatrix<3,3,double> Rzc_matrix(double alpha, double delta);
    QImage generate_image(double t1, double t2, double t3);
private:
    double focal_length;
    double number_of_points;
    QSize image_size;
    double alpha_1;
    double alpha_2;
    double beta;
    QGenericMatrix<3,3,double> rotation_mat;
    QGenericMatrix<3,3,double> translation_mat;
    QGenericMatrix<1,3,double> falling_beam_mat;
    QGenericMatrix<3,3,double> Rzc_mat;
    QGenericMatrix<3,3,double> Rx_mat;
    QGenericMatrix<3,3,double> Ry_mat;
    QGenericMatrix<3,3,double> Rzc2_mat;
    size_t line_width;
    QBrush brush_bluring;
};

#endif // IMAGEGENERATOR_H
