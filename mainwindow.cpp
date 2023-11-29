#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "angledeterminer1.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QTimer>
#include <QFileDialog>
#include <QCheckBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
//    , gen {174, 1000, QSize(700,700),-45,10}
    , gen {100, 1000, QSize(700,700),-45,10}
    , animation_timer {new QTimer(this)}
{
    ui->setupUi(this);

    //Set default generated image
    QImage img = gen.generate_image(0,0,0);
    ui->label->setPixmap(QPixmap::fromImage(img));

    // Connect animation logic
    connect(ui->btn_animate_t1, &QPushButton::toggled, ui->spn_set_t1, &QDoubleSpinBox::setDisabled);
    connect(ui->btn_animate_t2, &QPushButton::toggled, ui->spn_set_t2, &QDoubleSpinBox::setDisabled);
    connect(ui->btn_animate_t3, &QPushButton::toggled, ui->spn_set_t3, &QDoubleSpinBox::setDisabled);
    connect(ui->spn_set_t1, &QDoubleSpinBox::valueChanged, &animation_t1, &AngleChanger::setAngle);
    connect(ui->spn_set_t2, &QDoubleSpinBox::valueChanged, &animation_t2, &AngleChanger::setAngle);
    connect(ui->spn_set_t3, &QDoubleSpinBox::valueChanged, &animation_t3, &AngleChanger::setAngle);

    //Another controls logic
    connect(ui->btn_save, &QToolButton::pressed, this, &MainWindow::on_save_image);
    connect(ui->sld_line_width, &QSlider::valueChanged,this, &MainWindow::change_line_width);
    connect(ui->check_line_1, &QCheckBox::toggled, this, &MainWindow::set_lines_numbers);
    connect(ui->check_line_2, &QCheckBox::toggled, this, &MainWindow::set_lines_numbers);
    connect(ui->check_line_3, &QCheckBox::toggled, this, &MainWindow::set_lines_numbers);

    // Timer
    animation_timer->callOnTimeout(this, &MainWindow::animate);
    animation_timer->start(10);

    //Processing
    connect(ui->btn_start_proc, &QPushButton::clicked, this, &MainWindow::process);
    connect(ui->btn_count_angle_1, &QPushButton::clicked, this, &MainWindow::count_angle_1);
    connect(ui->btn_calc, &QPushButton::clicked, this, [this] () {
        QImage my_img;
        auto current_matrix = ImageProcessor::image2mat(image);

        for(int i =0; i <= 25; i++){
            AngleDeterminer det(current_matrix,i);
            qDebug() << det.result();
            if (i == 25) my_img = ImageProcessor::mat2image(det.resultImage());
        }
        ui->lbl_proc->setPixmap(QPixmap::fromImage(my_img));
        ui->lbl_proc->update();

    });

    set_lines_numbers();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::animate()
{
    if (ui->btn_animate_t1->isChecked())
    {
        animation_t1.change();
        ui->spn_set_t1->blockSignals(true);
        ui->spn_set_t1->setValue(animation_t1.getAngle());
        ui->spn_set_t1->ensurePolished();
        ui->spn_set_t1->blockSignals(false);
    }
    if (ui->btn_animate_t2->isChecked())
    {
        animation_t2.change();
        ui->spn_set_t2->blockSignals(true);
        ui->spn_set_t2->setValue(animation_t2.getAngle());
        ui->spn_set_t2->ensurePolished();
        ui->spn_set_t2->blockSignals(false);
    }
    if (ui->btn_animate_t3->isChecked())
    {
        animation_t3.change();
        ui->spn_set_t3->blockSignals(true);
        ui->spn_set_t3->setValue(animation_t3.getAngle());
        ui->spn_set_t3->ensurePolished();
        ui->spn_set_t3->blockSignals(false);
    }
    QImage new_image = gen.generate_image(animation_t1.getAngle(), animation_t2.getAngle(), animation_t3.getAngle());
    if (image != new_image)
    {
        image = new_image;
    }
    ui->label->setPixmap(QPixmap::fromImage(new_image));
}


void MainWindow::on_btn_animate_reset_clicked()
{
    // Reset all btns so angle do not changing
    ui->btn_animate_t1->setChecked(false);
    ui->btn_animate_t2->setChecked(false);
    ui->btn_animate_t3->setChecked(false);

    // Reset spinboxes so the angle is 0
    ui->spn_set_t1->setValue(0.0);
    ui->spn_set_t2->setValue(0.0);
    ui->spn_set_t3->setValue(0.0);
}

void MainWindow::on_save_image()
{
    QString path =  QFileDialog::getSaveFileName( this, tr("Сохранить изображение"), "", tr("Изображения (*.png *.jpg)"), nullptr, QFileDialog::DontUseNativeDialog);
   if (!path.isEmpty())
   {
        auto current_image =  gen.generate_image(animation_t1.getAngle(), animation_t2.getAngle(), animation_t3.getAngle());
        if(!current_image.save(path, "JPG",100))
        {
            throw std::runtime_error("Image was not saved");
        }
   }
}

void MainWindow::change_line_width(int new_width)
{
    gen.set_line_width(new_width);
}

void MainWindow::process()
{
   if (!image.isNull())
   {
       proccessed_image = image;
       proc.importImage(proccessed_image);
       proc.erodeImage(proc.original());
       proccessed_image = ImageProcessor::mat2image(proc.eroded());

       ui->lbl_proc->setPixmap(QPixmap::fromImage(proccessed_image, Qt::NoFormatConversion));
   }
}

double MainWindow::count_angle_1()
{
   auto current_matrix = ImageProcessor::image2mat(image);
   AngleDeterminer det(current_matrix, ui->spn_rotate->value());
   qDebug() << "counted angle is" << det.result().first << det.result().second;
   return det.result().first;
}

void MainWindow::set_lines_numbers()
{
   auto line_1 = ui->check_line_1->isChecked()? ImageGenerator::LINES::FIRST : 0;
   auto line_2 = ui->check_line_2->isChecked()? ImageGenerator::LINES::SECOND : 0;
   auto line_3 = ui->check_line_3->isChecked()? ImageGenerator::LINES::THIRD : 0;
   gen.unset_visible_lines();
   gen.set_visible_lines(line_1, line_2, line_3);
}


