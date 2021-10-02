
#include <QDir>
#include <Qdebug>

#include <opencv2/core/utils/logger.hpp>

MainWindow* theMainWindow = 0;

void ensureMakeDir(const QString& filepath)
{

}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    theMainWindow = this;

    ui->setupUi(this);
    // To display OpenCV debug on the console
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    // Define the type & condition of window(display)
    Qt::WindowFlags flags;
    flags = Qt::Dialog | Qt::FramelessWindowHint;
    setWindowFlags(flags);
    this->move(0,0);

    // from here
    m_appPath = QDir::currentPath();
    qDebug() << "MainWindow::MainWindow" << m_appPath;

    // swchoi

    configInit();

}


void MainWindow::coonfigInit()
{
    qDebug("Config Start");

    QString filename = appPath() +"/config.ini";
    m_configIni = new QSettings(filename, QSettings::IniFormat);


    mDisplayMode = m_configIni->value("LAYOUT/mode").toInt();


    // Using screenGeometry, the start coordinates of the Monitor obtains.
    QRect rec = QApplication::desktop()->screenGeometry();
    int height = rect.height();
    int width = rect.width();
    qDebug("[Mainwin] WindowSizw %dx%d",width,height);
    if( width > height ) {
        mWindowRec = 1;
        if( height > 600 ) {
            mWindowRec = 2;
            this->setGeometry(0,0,720,1080);
        }
        else {
            this->setGeometry(0,0,1024,600);
        }
        if( mDisplayMode == 1 ) {
            mWindowRec = 0;
            this->setGeometry(0,0,480,800);
        }
    }
    else {
        mWindowRec = 0;
        this->setGeometry(0,0,720,1350);
    }

    mWindowRec = 0; // Whatls the code above?
    this->setGeometry(0,0,720,1350);

    if ( mWindowRec == 1 ) {
	    qDebug("Set mWindowRec 1");
        ui->icon->move(1024-100,20);
        ui->labelId->move(210,180);
        ui->labelName->move(210,210);
        ui->labelTime1->move(750,120);
        ui->labelTime2->move(750,70);
        ui->fullScreen->resize(1024,600);
        ui->fullScreen->move(0,0);
        ui->eyeScreen->move(1024-280,600-240);
        ui->eyeScreen2->move(1024-280,600-240-200);
    }
    else if ( mWindowRec == 2 ) {
	    qDebug("Set mWindowRec 2");
        ui->icon->move(1024-100,20);
        ui->labelId->move(210,240);
        ui->labelName->move(210,270);
        ui->labelTime1->move(750,120);
        ui->labelTime2->move(750,70);
        ui->fullScreen->resize(720,720);
        ui->fullScreen->move(0,1080-720);
        ui->eyeScreen->move(0,1080-160);
        ui->eyeScreen2->move(0,1080-320);
	    mWindowRec = 0;
    }
    else {
        qDebug("Set mWindowRoc 0");
        ui->icon->move(720-140,0);
        ui->labelId->resize(120,50);

        ui->fullScreen->resize(720,1230);
        ui->fullScreen->move(0,50);
        ui->labelTime1->move(560,1220);
        ui->labelTime2->move(30,1220);

        ui->eyeScreen->resize(160,120);
        ui->eyeScreen->move(15,1280-120-50);
        ui->eyeScreen2->move(15,1280-120-50);

        ui->labelID->move(150,630);
        ui->labelName->move(210,270);
    }

    cv::VideoCapture icon;
    cv::Mat pic;
    QPixmap s_img;
    icon.open("./resource/j2c.png");
    icon >> pic; // I KNOW THIS WOULD WORK.
    cv::cvtColor(pic,pic,VC_BGR2RGB);
    cv::resize(pic,pic,cv::Size(120,45),0,0,0);
    s_img = QPixmap::formImage(QImage(pic.data, pic.cols, pic.rows, pic.step, QImage::Format_RGB888));
    ui->icon->setPixmap(s_img);

    QTimer::singleShot(1, this, SLOT(delayConfig()));
    QCursor cursor;
    cursor.setPos(720,1280);
}