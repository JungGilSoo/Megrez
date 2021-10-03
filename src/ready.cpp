

#include "ready.h"

#include <QDebug>

QImage faceGreen("./resource/face_g.png");
QImage faceBlue("./resource/face_b.png");
QImage faceRed("./resource/face_r.png");
int period;
int duty_on = 65;
int duty_off = 10;
int fd_period, fd_duty;

ReadyThread::ReadyThread(QObject* parent) : QThread(parent)
{
    char buff[100];
    int n;

    mCamOffsetX = theMainWindow->mCamOffsetX;
    mCamOffsetY = theMainWindow->mCamOffsetY;

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

int check_loop = 0;

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

int ReadyThread::faceCheck(cv::Mat camImg)
{
    int faceSize;
    int x,y,w,h;

    faceSize = 0;
    if ( m_state == mFace ) {
        check_loop++;
        if ( check_loop % 5 == 0 ) {
            cnt_t ++;
            faceSize = checkFaceDnn(camImg,1,&x,&y,&w,&h);
            if ( faceSize == 0 ) {
                qDebug("No Face %d cnt %d",noface_t,cnt_t);
                noface_t ++;
            }
            else {
                cnt_t = 0;
                noface_t = 0;
            }
        }

        if (faceSize > theMainWindow->mFaceDetecsize ) {
            m_state = mIdentify;
            cnt_t = 0;
            noface_t =0;
            return faceSize;
        }

        if ( (cnt_t > 20) && (noface_t > 10 )) {
            qDebug("[Ready] No Face");
            qDebug("No Face %d cnt %d",noface_t,cnt_t);
            if ( theMainwindow->mWindowRec == 1 ) {
                emit moveImage(0,0,0,1024,640);
            }
            if ( theMainWindow->mWindowRec == 2) {
                emit moveImage(0,0,60,1024,600);
            }

            m_state = mIdle;
            faceFlag = false;
            moveFlag = false;
            cnt_t = 0;
            noface_t = 0;
            backGround();
            return -1;
        }
        if ( cnt_t > 20) {
            cnt_t =0;
            noface_t = 0
        }
    }
    return faceSize;
}


int ReadyThread::readyDisplay()
{
    cv::Mat frame,org,img,camImg,procImg;

    int ret;

    webCam.read(camImg);
    if( camImg.empty() ){
        qDebug("WebCam Empty");
        return 0;
    }

    moveCheck(camImg);
    ret = faceCheck(camImg);
    if (ret > theMainWindow->mFaceDetectSize ) {
        theMainWindow->runIdentify();
    }
    if ( ret < 0 ) return ret;

    if ( m_state == mIdentify ) {
        faceSize = checkFaceDnn(camImg,1,&x,&y,&w,&h);
        qDebug("face x,y %d %d, w,h %d %d",x,y,w,h);
        cv::cvtColor(camImg, camImg,CV_BGR2RGB);

        if ( theMainWindow->mWindowRec == 0 ) {
            cv::resize(camImg,camImg,cv:size(1710,1280),0, 0, 0);
            camImg = camImg(cv::Rect((1710-720)/2-mCamOffsetX,0,720,1280));
        }
        cv::flip(camImg,procImg,1); // Why do flip it horizontally? 
        s_img = QPixmap::fromImg(QImge(procImg.dat, procImg.cols, procImg.rows, procImg.step, QImage::Format_RGB888));
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        if(faceSize > (theMainWindow->mFaceDetectSize + 50)) {
            painter.drawImage(0,mCamOffsetY,faceRed);
            theMainWindow->m_shutterPlayer->payIndex(6);
        }
        else if (faceSize < (theMainWindow->mFaceDetecSize + 30)) {
            painter.drawImage(0,mCamOffsetY,faceBlue);
//            theMainWindow->m_shutterPlayer->playIndex(5);
        }
        else
            painter.drawImge(0,mCamOffsetY,faceGreen);

        painter.end();    
    }
    else if ( m_state != mIdle ) {
        cv::cvtColor(camImg, camImg,CV_BFR2RGB);

        if ( theMainWindow->mWindowRec == 0) {
            cv::resize(camImg,camImg, cv::Size(1710,1280), 0, 0, 0);
            camImg = camImg(cv::Rect((1710-720)/2-mCamOffsetX,0,720,1280));
        }
        cv::flip(camImg,procImg,1);
        s_img = QPixmap::fromImage(QImage(procImg.data,procImg.cols,procimg.rows, procImg.step,QImage::Format_RGB888));

        QPainter painter(&s_img);
    }
}

int ReadyThead::checkFaceDnn(cv::Mat img, int opt, int *f_c,int *f_y,int *f_w, int *f_h)
{
    cv::Mat faceImg;
    int index = 0, maxWidth = 0;
    int scale = 2;

    cv::resize(img,faceImg,cv:Size(640/scale,480/scale),0, 0, 0);
    auto rectangles = m_face_detector->detect_face_rectangles(faceImg);
//    auto rectangles = m_face_detector->detect_face_rectangles(img);
    cv::Scalar color(0,200,250);
    if ( rectangles.size() > 0 ) {
        for(unsigned int i = 0; i<rectangles.size();i++) {
            rectangles[i].x *= scale;
            rectangles[i].y *= scale;
            rectangles[i].width *= scale;
            rectangles[i].height *= scale;
            if ( rectangles[i].width > maxWidth ) {
                maxWidth = rectanges[i].width;
                index = i;
                qDebug("Face(%d) width %d",i,rectangeles[i].width);
            }
            if (opt) cv::rectangle(img,rectangles[i],color,1);
        }
        if ( opt )
            qDebug("Face Size(%d) %d%d",index,rectangles[index].width, rectangels[index].height);

        *f_x = rectangles[index].x;
        *f_y = rectangles[index].y;
        *f_w = rectangles[index].width;
        *f_h = rectangles[index].height;
        return tectangles[index].width;    
    }
    //qDebug("Face Size %d", 0);
    return 0;
}