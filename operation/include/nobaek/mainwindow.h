#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <qtimer.h>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#include <ros/ros.h>
#include <std_msgs/String.h>
#include "da_msg/command.h"

#include "PhidgetBridge.h"
#include "PhidgetEncoder.h"
//#include "vaurora.h"
#include "vDX.h"

#include "dy_packet.h"
#include "thread1.h"
#include "thread2.h"
//#include "thread.h"
#include "estimator_fusion.h"

//math
#define PI                      (double)(3.14159265358979323846)
#define GRAVITY                 (double)(9.81)
#define DTR(x)                  (double)((double)(x) * PI / 180.0)	// degree to radian
#define RTD(x)                  (double)((double)(x) * 180.0 / PI)	// radian to degree
#define sq(x)                   (x*x)

//control
#define BENDING_PITCH  1
#define BENDING_YAW 2

#define del_time 1000000 //(1ms)
#define dt 0.01

//MOTOR ID
#define PITCH_1  2
#define PITCH_2  3
#define YAW_1 1
#define YAW_2 4
#define TRANSLATION  5
#define ROLL  6

#define ball_screw_lead 24
#define dynamixel_resolution 4096
#define LENGTH_TO_TIC (dynamixel_resolution/ball_screw_lead)


#define dynamixel_resolution 4096
#define MAPING_RATE 2
#define DOF         4


using namespace icra;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    vDX    motor;
    Thread1 thread1;
    Thread2 thread2;
    Thread3 thread3;
    PhidgetBridge loadcell;
    PhidgetEncoder encoder;
    Estimator estimator;


    QMessageBox msg_box;
    QTimer*  m_pTimer;
    QTimer*  timer;

    ros::Publisher aurora_pub;

    bool bThread_flag ;
    bool aurora_calibration_ok ;
    bool each_send_start;
    bool write_on;


private slots:
    void on_sin_signal_clicked();
    void on_conf_button_clicked();
    void on_motor_command_3_clicked();
    void update();
    void on_compensation_button_2_clicked();
    void on_read_motor_clicked();

    void on_STARTbutton_clicked();
    void on_close_all_clicked();
    void on_operation_start_clicked();
    void on_each_send_clicked();
    void on_motor_command_clicked();
    void on_STARTbutton_3_clicked();

    void on_nb_calibration_clicked();
    void on_ik_pose_button_clicked();
    void on_compensation_button_clicked();
    void on_pretension_end_click_clicked();
    void on_pretension_click_clicked();
    void on_STARTbutton_2_clicked();


    void on_datasave_button_clicked();

    void on_sin_stop_clicked();

    void on_operation_stop_clicked();

    void on_scnn_data_clicked();

    void on_gain_apply_clicked();

    void on_aurora_start_clicked();




private:
    Ui::MainWindow *ui;

};

extern MainWindow* pWnd;
extern vDX *pMotor;
extern Thread1 *pThread1;
extern Thread2 *pThread2;
extern Thread3 *pThread3;
extern PhidgetBridge* pLoadcell;
extern PhidgetEncoder* pEnc;
extern Estimator *pEstimator;

#endif // MAINWINDOW_H
