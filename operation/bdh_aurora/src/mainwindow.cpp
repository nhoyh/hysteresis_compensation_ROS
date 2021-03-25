#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow* pWnd;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pWnd = this;
    
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
    timer->start(100);

    glob_x = 0;
    glob_y = 0;
    glob_z = 0;
    glob_roll = 0;
    glob_pitch = 0;
    glob_yaw = 0;


    temp_x = 0;
    temp_y = 0;
    temp_z = 0;
    ref_x = 0;
    ref_y = 0;
    ref_z = 0;
    ref_roll = 0;
    ref_pitch = 0;
    ref_yaw = 0;
    result_x = 0;
    result_y = 0;
    result_z = 0;
    result_roll = 0;
    result_pitch = 0;
    result_yaw = 0;

    m_flag = 0;
    m_clutchflag = 0;

    aurora_init = false;
    aurora_pub  = false;
    for(int i=0;i<6;i++) aurora_cal_result[i] = 0.0;
    pitch_result = 0.0;
}

MainWindow::~MainWindow()
{

    delete timer;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(aurora->Init() == 1)
        msg_box.information(this,"Aurora","Successfully initialized");
    else
        msg_box.information(this,"Aurora","Error initializing");
}

void MainWindow::on_pushButton_2_clicked()
{
    if(aurora->ActivatePorts() == 1)
    {
        aurora->Beep(3);
        msg_box.information(this, "Aurora", "Port activated");
    }
    else
        msg_box.information(this, "Aurora", "Error port activating");
}

void MainWindow::on_pushButton_3_clicked()
{
    if(aurora->StartTracking() == 1)
    {
        msg_box.information(this, "Aurora", "Tracking started");
    }
    else
        msg_box.information(this, "Aurora", "Error start tracking");
}

void MainWindow::on_pushButton_4_clicked()
{
    if(aurora->StopTracking() == 1)
    {
        msg_box.information(this, "Aurora", "Tracking stopped");
    }
    else
        msg_box.information(this, "Aurora", "Error stop tracking");
}

void MainWindow::OnTimer()
{
    QString str;
    str = QString("%1").arg(aurora->x);
    ui->label->setText(str);
    str = QString("%1").arg(aurora->y);
    ui->label_2->setText(str);
    str = QString("%1").arg(aurora->z);
    ui->label_3->setText(str);
    str = QString("%1").arg(aurora->roll);
    ui->label_4->setText(str);
    str = QString("%1").arg(aurora->pitch);
    ui->label_5->setText(str);
    str = QString("%1").arg(aurora->yaw);
    ui->label_6->setText(str);

    str = QString("%1").arg(aurora_cal_result[0]);
    ui->cal_x_s->setText(str);
    str = QString("%1").arg(aurora_cal_result[1]);
    ui->cal_y_s->setText(str);
    str = QString("%1").arg(aurora_cal_result[2]);
    ui->cal_z_s->setText(str);
    str = QString("%1").arg(aurora_cal_result[3]);
    ui->cal_roll_s->setText(str);
    str = QString("%1").arg(aurora_cal_result[4]);
    ui->cal_pitch_s->setText(str);
    str = QString("%1").arg(aurora_cal_result[5]);
    ui->cal_yaw_s->setText(str);

    str = QString("%1").arg(pitch_result);
    ui->pitch_result_s->setText(str);

}


void MainWindow::on_calibration_clicked()
{
    m_clutchflag = 1;
    m_flag = 0;
    aurora_init = true;
}

void MainWindow::on_publisher_aurora_clicked()
{
    aurora_pub = true;
}

void MainWindow::on_publisher_aurora_stop_clicked()
{
    aurora_pub = false;
}
