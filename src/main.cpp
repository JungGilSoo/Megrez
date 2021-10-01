
#include "mainwindow.h"
#include <QApplication>

void serialConfigInit(); // What is this?

int main(int argc, int *argv)
{

    

    serialConfigInit(); //
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}