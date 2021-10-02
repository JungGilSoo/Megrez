

#include <QMainWindow>
#include <QSettings>


class MainWindow : public QMainWind
{

public:
    MainWindow (QWidgt *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;

    QString appPath() { return m_appPath; }

    int mWindowRect;
    int mDisplayMode;

private:

    QString m_appPath;


    void configInit();
}
