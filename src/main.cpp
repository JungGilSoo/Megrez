
#include <QApplication>

int main(int argc, int *argv)
{

    QApplication a(argc, argv);
    MainWindows w;
    w.show();

    return a.exec();
}