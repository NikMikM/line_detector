#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "imagegenerator.h"
#include "imageprocessor.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QTimer;
class QImage;

class AngleChanger : public QObject
{
    Q_OBJECT
public:
    AngleChanger()
      : angle {0.0},
        going_up {true},
        max_value{30}
    {}
    double getAngle() const
    {
        return angle;
    }
public slots:
    void change()
    {
        if ((angle >= -max_value) && (angle <= max_value)) {
            if (going_up) angle++;
            else angle--;
        }
        else {
            if (angle > max_value) {
                angle = max_value;
                going_up = false;
            }
            else{
                angle = -max_value;
                going_up = true;
            }
        }
    }
    void setAngle (double new_angle)
    {
        angle = new_angle;
    }

    void reset()
    {
        angle = 0;
        going_up = true;
    }

private:
    double angle;
    bool going_up;
    const short max_value;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void animate();
    void on_btn_animate_reset_clicked();
    void on_save_image();
    void change_line_width(int);
    void process();
    double count_angle_1();
    void set_lines_numbers();
private:
    Ui::MainWindow *ui;
    ImageGenerator gen;
    ImageProcessor proc;
    AngleChanger animation_t1, animation_t2, animation_t3;
    QImage image;
    QImage proccessed_image;
    QTimer* animation_timer;
};
#endif // MAINWINDOW_H
