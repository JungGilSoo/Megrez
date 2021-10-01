
#include <QApplication>

void serialConfigInit();

int main(int argc, int *argv)
{

    serialConfigInit();
    QApplication a(argc, argv);
    MainWindows w;
    w.show();

    return a.exec();
}