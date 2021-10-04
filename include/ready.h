#ifndef READYTHREAD_H
#define READYTHREAD_H

#include "facedetector.h"

#include <QThread>

class ReadyThread : public QThread
{
    Q_OBJECT
public:
    void backGround();  

    enum State
    {
        mStart=0,
        mIdle,
        mIdentify,
        mFace,
        mFaceIdentify,
    };
    
      
private:
    int webCamID;
    cv:VideoCapture webCam;
    cv::VideoCapture readycap;

    FaceDetector* m_face_detector = 0;

    int m_stop;
    int m_state;
    int readyDisplay();
    int faceDetec();

    bool moveFlag=false;
    bool faceFlag=false;

    int cnt_t = 0;
    int noface_t = 0;
    cv::Mat oldImg,newImg,diffImg;

    int mCamOffsetX, mCamOffsetY;
    int mFaceMode;
    int moveCheck(cv::Mat img);
    void irisDetecRun();
    void faceDetectRun();

signals:
    void readyDone();

protected:
    void run();
};

#endif // READYTHREAD_H