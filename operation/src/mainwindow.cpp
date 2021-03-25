#include "../include/nobaek/mainwindow.h"
#include "ui_mainwindow.h"
#include <sstream> // nhoyh

using namespace icra;

MainWindow* pWnd;
PhidgetBridge* pLoadcell;
PhidgetEncoder* pEnc;
//uEPOS2Control* pEposmotor;
Thread1 *pThread1;
Thread2 *pThread2;
Thread3 *pThread3;
//vAurora *pAurora;
vDX *pMotor;
FILE *fp;
FILE *f_FF_read;

//Thread *pThread;
Estimator *pEstimator;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pWnd        = this;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    timer->start(5);

    pMotor      = &motor;
    pLoadcell   = &loadcell;
    pEnc        = &encoder;

    pThread1    = &thread1;
    pThread2    = &thread2;
    pThread3    = &thread3;

    pEstimator = &estimator;

    bThread_flag = false;
    aurora_calibration_ok = false;
    each_send_start = true;

    write_on = false;

    ROS_INFO(" Init ");
    fp = fopen("/home/ldg/catkin_ws/src/optimizer_input/output.txt","w");

    //offset
   ui->offsetvalue_m1->setText(QString::number(0.0));
   ui->offsetvalue_m2->setText(QString::number(0.0));
   ui->offsetvalue_m3->setText(QString::number(0.0));
   ui->offsetvalue_m4->setText(QString::number(0.0));

   ui->com_feed_1->setText(QString::number(0.0));
   ui->com_feed_2->setText(QString::number(0.0));

   ui->com_d_th->setText(QString::number(0.0));
   ui->current_error_value->setText(QString::number(0.0));

   //load cell
   ui->load_m1_pitch->setText(QString::number(0.0));
   ui->load_m2_pitch->setText(QString::number(0.0));
   ui->load_m1_yaw->setText(QString::number(0.0));
   ui->load_m2_yaw->setText(QString::number(0.0));

   // motor current
   ui->motor_pitch1_current_value->setText(QString::number(0.0));
   ui->motor_pitch2_current_value->setText(QString::number(0.0));
   ui->motor_yaw1_current_value->setText(QString::number(0.0));
   ui->motor_yaw2_current_value->setText(QString::number(0.0));

   ros::start();
   ros::NodeHandle n;
   aurora_pub = n.advertise<da_msg::command>("aurora_start",1);


}

MainWindow::~MainWindow()
{
    delete timer;
    delete ui;

}

void MainWindow::update()
{
    if(bThread_flag==true && write_on==true) fprintf(fp, "%f, %f, %f, %f, %f, %f, %f\n",
                                                           thread2.time,
                                                           thread2.desired_posi[0],
                                                           thread2.desired_posi_dot[0],
                                                           estimator.estimated_angle,
                                                           estimator.ave_angle,
                                                           estimator.aurora_pitch,
                                                           thread2.Compensated_angle[0]
                                                           );


    // motor current
    ui->motor_pitch1_current_value->setText(QString::number(pThread2->LPF_motor_current[0])); //*PI/180
    ui->motor_pitch2_current_value->setText(QString::number(pThread2->LPF_motor_current[1])); //*PI/180
//    ui->motor_yaw1_current_value->setText(QString::number(pThread2->final_pos_value[YAW_1])); //*PI/180
//    ui->motor_yaw2_current_value->setText(QString::number(pThread2->final_pos_value[YAW_2])); //*PI/180


    // true angle from aurora
    ui->desired_angle->setText(QString::number(double(thread2.desired_posi[0])));
    ui->angle_aurora_vel->setText(QString::number(double(thread2.desired_posi_dot[0])));
    ui->actual_angle->setText(QString::number(double(estimator.aurora_pitch)));
    ui->current_error_value->setText(QString::number(double(thread1.current_error[0])));
    ui->com_d_th->setText(QString::number(double(thread2.Compensated_angle[0])));



    //load cell
    ui->load_m1_yaw->setText(QString::number(pThread2->loadcell_tension[0]));
    ui->load_m1_pitch->setText(QString::number(pThread2->loadcell_tension[1]));     // LDG - pitch up loadcell
    ui->load_m2_pitch->setText(QString::number(pThread2->loadcell_tension[2]));     // LDG - pitch down loadcell
    ui->load_m2_yaw->setText(QString::number(pThread2->loadcell_tension[3]));

    ui->error_m1_yaw->setText(QString::number(pThread2->pid_t.tension_err[0]));
    ui->error_m1_pitch->setText(QString::number(pThread2->pid_t.tension_err[1]));
    ui->error_m2_pitch->setText(QString::number(pThread2->pid_t.tension_err[2]));
    ui->error_m2_yaw->setText(QString::number(pThread2->pid_t.tension_err[3]));


    //configuration state
    ui->compensation_input_val->setText(QString::number(pThread2->Compensated_angle[0]));

    //compensation select end
    if(pThread2->compensation_end == true)
    {
        fclose(fp);
        pThread2->compensation_end = false;
    }

    ros::spinOnce();
}



void MainWindow::on_STARTbutton_clicked()
{
    ui->StatusCheck->insertPlainText(" initial motor ON!!\n");

    if(motor.Init()<0) ui->StatusCheck->insertPlainText(" Dynamixel init err!!\n");
    else
    {
        for(int id=1;id<5;id++) motor.SetTorqueEnable(0,id,1);
        usleep(5000);
        ui->StatusCheck->insertPlainText(" Dynamixel init success!!\n");
    }
}

void MainWindow::on_close_all_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Stop system", "Are you sure?", QMessageBox::Yes|QMessageBox::No);

    if(reply == QMessageBox::Yes)
    {
       this->close();
       ui->StatusCheck->insertPlainText(" Window Close \n");
       ui->StatusCheck->moveCursor(QTextCursor::End);

       for(int id=0;id<6;id++) motor.SetTorqueEnable(0,id+1,0);
       motor.Close();

       thread1.stop();
       thread2.stop();
       thread3.stop();

       fclose(fp);
       timer->stop();
       bThread_flag = false;
       sleep(0.5);
    }
    else if(reply == QMessageBox::No)
    {
       ui->StatusCheck->insertPlainText(" Keep this \n");
       ui->StatusCheck->moveCursor(QTextCursor::End);
    }
}


void MainWindow::on_operation_start_clicked()
{

    ui->StatusCheck->insertPlainText(" start operation \n");
    if(!bThread_flag)
    {
//        thread.start(); // aurora
        thread1.start(); //compensation control
        thread2.start(); //kinematics & trajectory
        thread3.start(); //dynamixel packet
        estimator.start();
        bThread_flag = true;
    }
}


void MainWindow::on_each_send_clicked()
{
    ui->StatusCheck->insertPlainText(" offset clicked!!\n");

    if(each_send_start ==true)
    {
        motor.nTarget[0][PITCH_1-1] = 2048;//motor.ReadData(PORT_NUMBER,TRANSLATION);
        motor.nTarget[0][PITCH_2-1] = 2048;
        motor.nTarget[0][YAW_1-1] = 2048;
        motor.nTarget[0][YAW_2-1] = 2048 ;
        motor.SyncWrite(0);

        sleep(1.5);
        each_send_start = false;
    }
}


void MainWindow::on_motor_command_clicked()
{
    ui->StatusCheck->insertPlainText(" click motor command!\n");

    double d_value[4] = {0.0,};

    d_value[0] = ui->motor_pitch->text().toDouble();
    d_value[1] = ui->motor_yaw->text().toDouble();
    d_value[2] = ui->motor_roll->text().toDouble();
    d_value[3] = ui->motor_trans->text().toDouble();

    thread2.desired_posi[0] = d_value[0];
    thread2.desired_posi[1] = d_value[1];
    thread2.desired_posi[2] = d_value[2];
    thread2.desired_posi[3] = d_value[3];

    thread1.error_sum[0] =0.0;
}



void MainWindow::on_nb_calibration_clicked()
{
    ui->StatusCheck->insertPlainText(" nb calibaration\n");
    pThread2->sin_magnitude = ui->sin_mag->text().toDouble();
    pThread2->conf_calibration = true;
    write_on = true;
}


void MainWindow::on_ik_pose_button_clicked()
{
    ui->StatusCheck->insertPlainText("ik_pose_estimation_pub\n");
    pThread2->ik_pose_pub_start= true;

}

void MainWindow::on_compensation_button_clicked() //feedback
{
    ui->StatusCheck->insertPlainText("feedback compensation on\n");
    pThread1->compensator_on = true;
}

vector<string> split(string input, char delimiter) {
    vector<string> answer;
    stringstream ss(input);
    string temp;

    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }

    return answer;
}

void MainWindow::on_compensation_button_2_clicked()
{
    ui->StatusCheck->insertPlainText("Feed forward on\n");
    pThread2->feed_forward_on= true;
    f_FF_read = fopen("/home/ldg/catkin_ws/src/optimizer_input/optimized_result.txt","r");
    char str[100];
    fgets(str,100,f_FF_read);
    fclose(f_FF_read);

    vector<string> arr = split(str, ',');
    ui->StatusCheck->insertPlainText("Feed forward end\n");

    for(int i=0;i<6;i++) pThread2->FF.param[i] = stod(arr[i]);
    usleep(1000);
    pThread2->feed_forward_start = true;
//    write_on = true;
//    qDebug() << "optim param=" << pThread2->FF.param[0] << pThread2->FF.param[1];
}



void MainWindow::on_read_motor_clicked()
{
    ui->StatusCheck->insertPlainText("motor read!\n");
    ui->offsetvalue_m1->setText(QString::number(int(motor.ReadData(0,1))));
    ui->offsetvalue_m2->setText(QString::number(int(motor.ReadData(0,2))));
    ui->offsetvalue_m3->setText(QString::number(int(motor.ReadData(0,3))));
    ui->offsetvalue_m4->setText(QString::number(int(motor.ReadData(0,4))));
    usleep(5000);
}


void MainWindow::on_pretension_click_clicked()
{
    double init_force =5.0;  // LDG - pretension value : 5[N]

    ui->StatusCheck->insertPlainText("pretension applied \n");
    thread2.pre_tension_onoff = true;
    thread2.pid_t.desired_pre_tension[0] = init_force;
    thread2.pid_t.desired_pre_tension[1] = init_force;
    thread2.pid_t.desired_pre_tension[2] = init_force;
    thread2.pid_t.desired_pre_tension[3] = init_force;
}

void MainWindow::on_pretension_end_click_clicked()
{
    ui->StatusCheck->insertPlainText("pretension end\n");
    thread2.pre_tension_onoff = false;
}

void MainWindow::on_STARTbutton_2_clicked()
{
    //load cell
    if(loadcell.InitDevice() != false)
        ui->StatusCheck->insertPlainText("loadcell ON\n");
    else
        ui->StatusCheck->insertPlainText("loadcell not init\n");
}

void MainWindow::on_STARTbutton_3_clicked()
{
    //encoder
//    if(encoder.InitDevice() != false)
//        ui->StatusCheck->insertPlainText("master ON\n");
//    else
//        ui->StatusCheck->insertPlainText("master not init\n");
}

void MainWindow::on_motor_command_3_clicked()
{
    ui->StatusCheck->insertPlainText("master on\n");
    pThread2->master_on = true;
    write_on = true;
}

void MainWindow::on_conf_button_clicked()
{
    ui->StatusCheck->insertPlainText("feedforward select\n");
    pThread2->feed_forward_on= true;

}

void MainWindow::on_sin_signal_clicked()
{
    ui->StatusCheck->insertPlainText(" sin trajectory\n");
    pThread2->sin_magnitude = ui->sin_mag->text().toDouble();
    pThread2->start_sin_traj = true;
    write_on = true;
}

void MainWindow::on_datasave_button_clicked()
{
    ui->StatusCheck->insertPlainText(" data save on\n");
    //pThread2->data_save_on = true;
    pThread2->pub_image_screen_shot();
}

void MainWindow::on_sin_stop_clicked()
{
    ui->StatusCheck->insertPlainText(" sin trajectory - stop\n");
    pThread2->traj_stop = true;
    write_on = false;
}

void MainWindow::on_operation_stop_clicked()
{
    ui->StatusCheck->insertPlainText(" stop operation \n");
    if(bThread_flag)
    {
        thread1.stop(); //compensation control
        thread2.stop(); //kinematics & trajectory
        thread3.stop(); //callback
        estimator.start();
        bThread_flag = false;
    }
}

void MainWindow::on_scnn_data_clicked()
{
    ui->StatusCheck->insertPlainText(" scnn data save on\n");
    pThread2->scnn_data_flag = true;
}

void MainWindow::on_gain_apply_clicked()
{
    ui->StatusCheck->insertPlainText(" click motor command!\n");

    double gain[3] = {0.0,};  //3 , 1.3, 0 (P, I, D)

    gain[0] = ui->p_gain->text().toDouble();
    gain[1] = ui->d_gain->text().toDouble();
    gain[2] = ui->i_gain->text().toDouble();


    thread1.gain_[0] = gain[0]; //p
    thread1.gain_[1] = gain[1]; //d
    thread1.gain_[2] = gain[2]; //i

}

void MainWindow::on_aurora_start_clicked()
{
    ui->StatusCheck->insertPlainText(" aurora start pub\n");
    estimator.aurora_callback_start = true;
}
