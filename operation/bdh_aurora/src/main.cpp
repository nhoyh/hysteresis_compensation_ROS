#include "mainwindow.h"
#include <QApplication>
#include <ros/ros.h>
#include "vaurora.h"


int main(int argc, char *argv[])
{
    ros::init(argc, argv,"BDH_Aurora");

    QApplication a(argc, argv);
    MainWindow w;
    vAurora ar;
    w.aurora = &ar;
    w.show();

    return a.exec();
}





