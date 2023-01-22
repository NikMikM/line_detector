#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QTimer>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , gen {174, 1000, QSize(700,700),-45,10}
    , animation_timer {new QTimer(this)}
{
    ui->setupUi(this);
    //Set default generated image
    QImage img = gen.generate_image(0,0,0);
    ui->label->setPixmap(QPixmap::fromImage(img));
    // Set line width for image gen
    gen.set_line_width(6);

    // Connect animation logic
    connect(ui->btn_animate_t1, &QPushButton::toggled, ui->spn_set_t1, &QDoubleSpinBox::setDisabled);
    connect(ui->btn_animate_t2, &QPushButton::toggled, ui->spn_set_t2, &QDoubleSpinBox::setDisabled);
    connect(ui->btn_animate_t3, &QPushButton::toggled, ui->spn_set_t3, &QDoubleSpinBox::setDisabled);
    connect(ui->spn_set_t1, &QDoubleSpinBox::valueChanged, &animation_t1, &AngleChanger::setAngle);
    connect(ui->spn_set_t2, &QDoubleSpinBox::valueChanged, &animation_t2, &AngleChanger::setAngle);
    connect(ui->spn_set_t3, &QDoubleSpinBox::valueChanged, &animation_t3, &AngleChanger::setAngle);
    connect(ui->btn_save, &QToolButton::clicked, this, &MainWindow::on_save_image);

    // Timer
    animation_timer->callOnTimeout(this, &MainWindow::animate);
    animation_timer->start(10);

    //Processing
    connect(ui->btn_start_proc, &QPushButton::clicked, this, &MainWindow::process);
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
   QString path =  QFileDialog::getSaveFileName( this, tr("Сохранить изображение"), "", tr("Изображения (*.png *.jpg") );
   if (!path.isEmpty())
   {
       image.save(path, nullptr, 100);
   }
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
