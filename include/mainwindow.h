#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "shutterplayer.h"
#include <QMainWindow>
#include <QSettings>

class IrisDevice;
class ReadyThread;

class MainWindow : public QMainWind
{

public:
    MainWindow (QWidgt *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;

    QString appPath() { return m_appPath; }

    ShutterPlayer* m_shutterPlayer;

    QSetting* m_configIni;

    IrisDevice* m_irisDevice = 0;
    QString mFaceDetectMode;
    int mWindowRec;
    int mMoveThreashold;
    int mDisplayMode;
    int mCamWidth, mCamHeight;
    int mCamRunWidth, mCamRunHeight;
    int mCamOffsetX, mCamOffsetY;
    int mUsbType;

private:

    QString m_appPath;

    ReadyThread* m_readyThread=0;

    void initIrisDevice();

    void configInit();

private slots:

    void connectedIrisDevice(QString);

    void delayConfig();
}

extern MainWindow* theMainWindow;

#endif // MAINWINDOW_H