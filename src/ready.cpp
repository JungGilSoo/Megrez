

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

void ReadyThread::faceDetectRun()
{
    qDebug("faceDetectRun");
    for(;;) {
        if(m_state == mStart) {
            backGroud();
            m_state = mIdle;
            QThread::msleep(50);
//            continue;
        }

        if(m_state == mface || m_state == mFaceIdentify || m_state == mIdentify) {
            QThread::msleep(20);
        }
        else {
            QThread::msleep(350);
        }

        if(m_stop) break;

        int ret = faceDetec();
        if(ret == 0) {
            qDebug("ready display done");
            QThread::msleep(100);
            emit readyDone();
            break;
        }
    }

    QPixmap s_img;
    emit drawImage(0,s_img);
    if(webCam.isOpend()) webCam.release();
    qDebug("[Ready] release");
}

void ReadyThread::irisDetecRun()
{
    for(;;) {
        int ret = readyDisplay();
//        emit theMainWindow->msgBox("WebCam Read Error");
        if ( ret == 0 ) {
//        emit theMainwindow->msgBox("WebCam read Error");
            qDebug("ready display done");
            QThread::mslee(100);
            emit readyDone();
        }

       if ( m_state == mStart ) {
          backGround();
           m_state = mIdle;
           QThread::msleep(100);
        }
       else if ( m_state == mIdentify ) {
            QThread::msleep(40);
            qDebug("[Ready] mIdentify state");
        }
        else if (m_state == mFace) {
            QThread::msleep(20);
        }
        else{
            QThread::msleep(350);
        }

        if ( m_stop) break;
    }
    QPixmap s_img;
    emit drawImage(0,s_img);
    if (webCam.isOpend()) webCam.release();
    qDebug("[Ready] release");
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

int faceRetryCnt = 0;
int ReadyThread::faceDetec()
{
    cv::Mat org,camImg,procImg;
    QPixmap s_img;
    /*
    if (m_state == mStart) {
        return 1;
    }
    */

    webCam.read(camImg);
    if (camImg.empty()) {
        qDebug("WebCam Empty");
        return 0;
    }

    cv::flip(camImg,camImg,0);
    camImg.copyTo(org);

    if (m_state <= mIdle){
        faceRetryCnt = 0;
        ret = moveCheck(camImg);
        return 1; // move : m_state == mFace
    }

    if(m_state == mFace) {
        ret = faceCheck(camImg)
    }
    else
        ret = checkFaceDnn(camImg,0,&x,&y,&w,&h);

    cv::cvtColor(camImg, camImg, CV_BGR2RGB);

    if ( theMainWindow->mWindowRec == 0 ) {
        cv::resize(camImg, camImg,cv::Size(1710,1280),0, 0, 0);
        caImg = camImg(cv::Rect((1710-720/2-mCamOffsetX),0,720,1280));
    }

    cv::flip(camImg,procimg,1);
    s_img = QPixmap::fromImage(QImage(procImg.dat, procImg.cols,procImg.rows,procImg.step,QImage::Format_RGB888));

    QPainter painier(&s_img);
    painier.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.end();
    emit drawImage(0,s_img);

    qDebug("Face x,y %d %d, w,h %d %d",x,y,w,h);
    if (ret > theMainWindow->mFaceDetectSize) {
//        cv::cvtColor(org, org, CV_BGR2RGB);
        qDebug("ProcImg %dx%d",org.clos,org.rows);
//        cv::resize(procImg,procImg,cv::Size(procImg.cols/3,procImg.rows/3),0, 0, 0);
        ret = identifyFace(org,x,y,w,h);
        faceIdentifycnt = 0;
        if ( ret 1= 1) {
            faceRetryCnt++;
            if(faceRetryCnt > 5) {
                m_state = mStart;
                m_stop = true;
                theMainWindow->showIdentifyFail();
            }
        }
    }
    else {
        if (faceIdentifycnt ++ > 50) {
            m_state = mIdle;
            faceFlag = false;
            moveFlag = false;
            faceIdentifycnt = 0;
            cnt_t = 0;
            noface_t = 0;
            backGroud()
        }
    }
    return 1;
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
        painier.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(0,mCamOffsetY,faceStart);

        painter.end();
    }
    else {
        return 1;

        readycap >> org;
        if (irg.empy()) {
            qDebug("ready mov empty");
            readyCap.release();
            readycap.open("resource/ready.mov");
            readycap >> org;
        }
        if ( theMainwindow->mWindowRec) {
            cv::resize(org,org,cv::Size(1024,600),0,0,0);
            frame = org(cv::Rect(0,0,1024,600));
        }
        else
        {
            cv::rotate(org,org,cv::ROTATE_90_CLOCKWISE);
	        /*
            cv::resize(org,org,cv::Size(720,1280),0, 0, 0);
            frame = org(cv::Rect(0,(1280-1000)/2,720,1000));
	        */
            cv::resize(org,frame,cv::Size(720,1280),0, 0, 0);      
        }
        cv::cvtColor(frame, frame,CV_BGR2RGB);
//        frame = frame(cv::Rect(0,0,600,600));
//        cv::resize(frame,frame,cv::Size(720,720),0,0,0);
        s_img = QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888));
        current = QDateTime::currentDateTime();
        if ( current.time().second() %2 == 0 ) {
	    QPainter painter(&s_img);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            if ( theMainWindow->mWindowRec ) {
                painter.drawImage(110,60,imgTouchScreen);
	    }
	    else {
                painter.drawImage(0,(1000-720)/2,imgTouchScreen);
	    }
            painter.end();
        }
    }

    emit drawImage(0,s_img);
    return 1;
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


extern int grpcIdentify(int type,cv::Mat img,std::string *id,std::string *name);

int ReadyThread::identifyFace(cv::Mat ing,int x,int y,int w,int h)
{
    int ret;
    std::string id, namw;

    ret = grpcIdentify(0,img,&id,&name);
    qDebug("grpc ret %d",ret);
    if(ret == 1) {
        theMainWindow->showIdentifySuccess(id.c_str(),name.c_str());
        qDebug("id %s name %s",id.c_str(),name.c_str());
        m_state = mStart;
        m_stop = true;
        /*
        faceFlay = false;
        moveFlay = false;
        faceIdentifycnt = 0;
        cnt_t = 0;
        noface_t = 0;
        backGround();
        */
    }
    else {
        qDebug("Identify Fail %d",ret);
    }
    return ret;
}