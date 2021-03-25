#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qtimer.h>
#include <QMessageBox>
#include <QTimer>
#include "vaurora.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    vAurora*    aurora;
    QMessageBox msg_box;
    QTimer*     timer;

    double    glob_x;
    double    glob_y;
    double    glob_z;
    double    glob_roll;
    double    glob_pitch;
    double    glob_yaw;

    double    temp_x;
    double    temp_y;
    double    temp_z;
    double    temp_roll;
    double    temp_pitch;
    double    temp_yaw;

    double    ref_x;
    double    ref_y;
    double    ref_z;
    double    ref_roll;
    double    ref_pitch;
    double    ref_yaw;

    double    result_x;
    double    result_y;
    double    result_z;
    double    result_roll;
    double    result_pitch;
    double    result_yaw;

    int       m_flag;
    int       m_clutchflag;

    bool aurora_init;
    bool aurora_pub;

    double aurora_cal_result[6];
    double pitch_result;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void OnTimer();

    void on_calibration_clicked();

    void on_publisher_aurora_clicked();

    void on_publisher_aurora_stop_clicked();

private:
    Ui::MainWindow *ui;
};

extern MainWindow* pWnd;
#endif // MAINWINDOW_H
