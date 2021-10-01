
#include "mainwindow.h"
#include <QApplication>
#include <QTime>

void serialConfigInit(); // What is this?

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    QTime curtime = QTime::currentTime();

    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "[%s:%03d] %s\n",curtime.toString().toStdString().c_str(), curtime.msec(),localMsg.constData());
        break;
    case QtInfoMsg:
        fprintf(stderr, "[%s:%d] Info %s\n",curtime.toString().toStdString().c_str(), curtime.msec(),localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }
}

int main(int argc, int *argv)
{

    printf("[Start]\n");
    qInstallMessageHandler(myMessageOutpur); // Message Handler Registration
    serialConfigInit(); //
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}