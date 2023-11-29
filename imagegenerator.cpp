#include "imagegenerator.h"

#include <QGenericMatrix>
#include <QPainter>
#include <QPainterPath>
#include "angledeterminer1.h"
#include <cmath>
#include <random>

Q_GUI_EXPORT void qt_blurImage( QImage& blurImage, qreal radius, bool quality, int transposed = 0 );
Q_WIDGETS_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);

using namespace std;

ImageGenerator::ImageGenerator(double focal_length, double number_of_points, QSize img_size, double alpha1, double beta):
    focal_length {focal_length},
    number_of_points {number_of_points},
    image_size {img_size},
    alpha_1 {alpha1},
    beta {beta},
    line_width {3}
{
    double trans_values[] = {-1 / sqrt(2), 1/sqrt(2),0,
                             -1 / sqrt(6), -1 / sqrt(6), 2 / sqrt(6),
                             1 / sqrt(3), 1 / sqrt(3), 1 / sqrt(3) };
    translation_mat = QGenericMatrix<3,3,double>{trans_values};

    double rx_values[] = {-1, 0, 0,
                          0, 1, 0,
                          0, 0, 1};
    Rx_mat = QGenericMatrix<3,3,double>{rx_values};
    double ry_values[] = {1, 0, 0,
                          0, -1, 0,
                          0, 0, 1};
    Ry_mat = QGenericMatrix<3,3,double>{ry_values};
    double falling_beam_values[] = {0,0,-1};
    falling_beam_mat = QGenericMatrix<1,3,double>{falling_beam_values};
//    // Brush settings
//    QRadialGradient grad(QPointF(0,0), 500);
//    grad.setColorAt(0,Qt::white);
//    grad.setColorAt(5,Qt::black);

//    brush_bluring = QBrush(grad);
//    brush_bluring.setStyle(Qt::RadialGradientPattern);
//    brush_bluring.setColor(Qt::white);
}

ImageGenerator::ImageGenerator(double focal_length, double number_of_points, QSize img_size, double alpha1, double alpha2, double beta):
    focal_length {focal_length},
    number_of_points {number_of_points},
    image_size {img_size},
    alpha_1 {alpha1},
    alpha_2 {alpha2},
    beta {beta},
    line_width {3}
{
    double trans_values[] = {-1 / sqrt(2), 1/sqrt(2),0,
                             -1 / sqrt(6), -1 / sqrt(6), 2 / sqrt(6),
                             1 / sqrt(3), 1 / sqrt(3), 1 / sqrt(3) };
    translation_mat = QGenericMatrix<3,3,double>{trans_values};

    double rx_values[] = {-1, 0, 0,
                          0, 1, 0,
                          0, 0, 1};
    Rx_mat = QGenericMatrix<3,3,double>{rx_values};
    double ry_values[] = {1, 0, 0,
                          0, -1, 0,
                          0, 0, 1};
    Ry_mat = QGenericMatrix<3,3,double>{ry_values};
    double falling_beam_values[] = {0,0,-1};
    falling_beam_mat = QGenericMatrix<1,3,double>{falling_beam_values};
    // Brush settings
    brush_bluring = QBrush(Qt::white,Qt::RadialGradientPattern);

}

ImageGenerator::~ImageGenerator()
{

}
void ImageGenerator::set_rotation_matrix(double t1, double t2, double t3)
{
    t1 = qDegreesToRadians(t1);
    t2 = qDegreesToRadians(t2);
    t3 = qDegreesToRadians(t3);
    double values2[] = {qCos(t2) * qCos(t3), -qCos(t2) * qSin(t3),qSin(t2),
                        qCos(t1) * qSin(t3) + qCos(t3) * qSin(t1) * qSin(t2), qCos(t1) * qCos(t3) - qSin(t1) * qSin(t2) * qSin(t3), -qCos(t2) * qSin(t1),
                        qSin(t1) * qSin(t3) - qCos(t1) *qCos(t3) * qSin(t2), qCos(t3) * qSin(t1) + qCos(t1) * qSin(t2) * qSin(t3), qCos(t1) * qCos(t2)
                       };
    rotation_mat = QGenericMatrix<3,3,double>{values2};
}

QGenericMatrix<3, 3, double> ImageGenerator::Rzc_matrix(double alpha, double delta)
{
    alpha = qDegreesToRadians(alpha);
    delta = qDegreesToRadians(delta);
    double Nx = qCos(alpha) * qSin(delta);
    double Ny = qSin(alpha) * qSin(delta);
    double Nz = qCos(delta);
    double values[] = {1 - 2 * pow(Nx,2), -2 * Nx * Ny, -2 * Nx * Nz,
                       -2 * Nx * Ny, 1 - 2 * pow(Ny,2), -2 * Ny * Nz,
                       -2 * Nx * Nz, -2 * Ny * Nz, 1 - 2 * pow(Nz,2)};
    return QGenericMatrix<3,3,double>{values};
}

QImage ImageGenerator::generate_image(double t1, double t2, double t3)
{
    // Caclculate points on image according to specified beta angle, number_of_points and angles t1, t2, t3
    set_rotation_matrix(t1,t2,t3);
    QVector<QPointF> pts_horizontal, pts_vertical;
    QPointF pt123, pt321, pt231, pt132;

    for(size_t i{0}; i < number_of_points; i++){
        double delta = i * beta / number_of_points;
        Rzc_mat = Rzc_matrix(alpha_1, delta);
        auto B123 = rotation_mat * translation_mat * Rzc_mat * Ry_mat * Rx_mat * translation_mat.transposed() * rotation_mat.transposed() * falling_beam_mat;
        auto B231 = rotation_mat * translation_mat * Rx_mat * Rzc_mat * Ry_mat * translation_mat.transposed() * rotation_mat.transposed() * falling_beam_mat;
        auto B321 = rotation_mat * translation_mat * Rx_mat * Ry_mat * Rzc_mat * translation_mat.transposed() * rotation_mat.transposed() * falling_beam_mat;
        auto B132 = rotation_mat * translation_mat * Ry_mat * Rzc_mat * Rx_mat * translation_mat.transposed() * rotation_mat.transposed() * falling_beam_mat;

        pt123 = QPointF(focal_length * (B123(0,0)) / (B123(2,0)) ,
                        focal_length * qTan(qAsin(B123(1,0))));
        pt321 = QPointF(focal_length * (B321(0,0)) / (B321(2,0)) ,
                        focal_length * qTan(qAsin(B321(1,0))));
        pt231 = QPointF(focal_length * (B231(0,0)) / (B231(2,0)) ,
                        focal_length * qTan(qAsin(B231(1,0))));
        pt132 = QPointF(focal_length * (B132(0,0)) / (B132(2,0)) ,
                        focal_length * qTan(qAsin(B132(1,0))));

        pts_vertical.push_back(pt231);
        pts_vertical.push_front(pt132);
        pts_horizontal.push_back(pt123);
        pts_horizontal.push_front(pt321);
    }

    // Draw image
    QImage result{image_size, QImage::Format_Grayscale8};
    QPainter painter(&result);
    painter.fillRect(result.rect(),Qt::black);

    // Translate, scale and change coordinate system to decard
    painter.translate(image_size.width()/2,image_size.width()/2);

    double scale_factor = 6;//image_size.width() / (2 * abs(pts_horizontal[0].y()));

    for(QPointF& pt : pts_horizontal){
        pt = QPointF(pt.x() * scale_factor, -pt.y() * scale_factor);
    }
    for(QPointF& pt : pts_vertical){
        pt = QPointF(pt.x() * scale_factor, -pt.y() * scale_factor);
    }

    // Fill QPainterPaths with line
    QPainterPath vertical_line(pts_vertical[0]);
    for (auto pt : qAsConst(pts_vertical))
    {
        vertical_line.lineTo(pt);
    }
    QPainterPath horizontal_line(pts_horizontal[0]);
    for (auto pt : qAsConst(pts_horizontal))
    {
        horizontal_line.lineTo(pt);
    }

    painter.setPen(QPen(Qt::white,line_width));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::VerticalSubpixelPositioning | QPainter::NonCosmeticBrushPatterns);
//    painter.setRenderHint(QPainter::NonCosmeticBrushPatterns);
//    painter.setRenderHint(QPainter::VerticalSubpixelPositioning);

//    QConicalGradient gradient;
//            gradient.setCenter(result.rect().center());
//            gradient.setAngle(90);
//            gradient.setColorAt(1.0, Qt::black);
//            gradient.setColorAt(0.0, Qt::white);

//            auto p = QPen(gradient, 4.0);
//            painter.setPen(p);
    //Drawing
    if (visible_lines & FIRST)
    painter.drawPath(horizontal_line);
    if(visible_lines & SECOND)
    painter.drawPath(vertical_line);

//    std::random_device rd;  // a seed source for the random number engine
//    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
//    std::uniform_int_distribution<> distrib(0,50);
//    for(auto i = 0; i < image_size.width(); i++)
//    {
//    for(auto j = 0; j < image_size.height();j++) {
//        auto num = distrib(gen);
//        const auto in_pix = result.pixel(i,j);
//        if( in_pix + num < 255)
//            result.setPixel(i,j,in_pix + num);

//    }
//    }
    return result;
}

//void ImageGenerator::unset_visible_lines()
//{
//    visible_lines = 0;
//}

