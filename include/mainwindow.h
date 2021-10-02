

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

    QSetting* m_configIni;

    IrisDevice* m_irisDevice = 0;
    QString mFaceDetectMode;
    int mWindowRec;
    int mMoveThreashold;
    int mDisplayMode;
    int mCamWidth, mCamHeight;

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
