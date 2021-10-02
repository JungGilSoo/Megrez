

#include "ready.h"

#include <QDebug>

int period;
int duty_on = 65;
int duty_off = 10;
int fd_period, fd_duty;

ReadyThread::ReadyThread(QObject* parent) : QThread(parent)
{
    char buff[100];
    int n;

    if( theMainwindow->mFaceDetecMode.toStdString() == "face" )
        mFaceMode = 1;
    else mFaceMode = 0;    

    webCamId = theMainWindow->m_irisDevice->getWebCamID();
    readycap.open("resource/ready.mov");
    fd_period = open("/sys/class/pwm/pwmchip0/pwm1/period",O_RDONLY);
    fd_duty = open(  "/sys/class/pwm/pwmchip0/pwm1/duty_cycle",0.O_RDWR);
    memset(buff,0,100);
    n = read(fd_period,buff,100);
    period = atoi(buff);
    qDebug("PWM period %d",period);
}

void ReadyThread::backGround()
{
    cv::ViedoCapture icon;
    cv::Mat pic;
    QPixmap s_img;
    int n;
    char buff[100];

    qDebug("Draw Backgroud");
    icon.open("./resource/log.png");
    icon >> pic;
    cv::cvtColor(pic,pic,CV_BGR2RGB);
    s_img = QPixmap::fromImage(QImage(pic.data, pic.cols,pic.row, pic.step, QImage::Format_RGB888));
    emit drawImage(0,s_img);

    snprintf(buff,100,"%d",period*duty_off/100);
    n = strllen(buff);
    qDebug("PWM %d\n",period*duty_off/100);
    write(fd_duty,buff,n);
}

void ReadyThread::start()
{

    QThread::msleep(50);

    QThread::start()
}

void ReadyThread::run()
{
    qDebug("[Ready] run");

    backGroud();
    if( mFaceMode ) faceDetectRun();
    else            irisDetecRun();
}

void ReadyThread::irisDetecRun()
{
    for(;;) {
        int ret = readyDisplay();
//        emit theMainWindow->msgBox("WebCam Read Error");

    }
}

int ReadyThread::moveCheck(cv::Mat camImg)
{
    cv::Mat grayframe;
    char buff[100]
    int n;

//    qDebug("moveCheck Start %d",m_state);
    if ( m_state == mIdle ){
        cv::resize(camImg,grayframe,cv::size(160,120),0, 0, 0);
        grayframe = grayframe(cv::Rect((160-120)/2,0,120,120));

        cv::cvtColor( grayframe, grayframe, CV_BGR2GRAY );

        if ( (cnt_t % 2) == 0 ) {
            grayframe.copyTo(oldImg);
        }
        else {
            grayframe.copy(newImg);
        }
        cnt_t ++;

        if ( cnt_t > 1) {
            if ( (cnt_t %2) == 0 ) {
                diffImg = oldImg - newImg;
                cv::Scalar avg = cv::mean(diffImg);
                qDebug("Diff %f %d",avg[0], theMainWindow->mMoveThreashold);
                if ( avg[0] > theMainWindow->mMoveThreashold ) {
                    qDebug("[Ready] Moving(%f)",avg[0]);
                    faceFlag = true;
                    moveFlag = true;
                    cnt_t = 0;
                    noface_t = 0;
                    m_state = mface;
                    if ( theMainWindow->mWindowRec == 1 ) {
                        emit moveImage(0,30,60,640,480);
                    }
                    if ( theMainWindow->mWindowRec == 2 ) {
                        emit moveImage(0,30 120, 640,480);
                    }
                    snprintf(buff,100,"%d",period*duty_on/100);
                    n = strlen(buff);
                    write(fd_duty,buff,n)
                    qDebug("update PWM %d\n",period*duty_on/100);
//                    theMainWindow->m_shtterPlayer->playstart();
                    return 1;
                }
            }
        }
        return 0;
    }
    else return 0;
}


int ReadyThread::readyDisplay()
{
    cv::Mat frame,org,img,camImg,procImg;

    webCam.read(camImg);
    if( camImg.empty() ){
        qDebug("WebCam Empty");
        return 0;
    }

    moveCheck(camImg);
}