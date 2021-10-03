#ifndef READYTHREAD_H
#define READYTHREAD_H

#include "facedetector.h"

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

    int m_state;

    bool moveFlag=false;
    bool faceFlag=false;

    int cnt_t = 0;
    int noface_t = 0;
    cv::Mat oldImg,newImg,diffImg;

    int mCamOffsetX, mCamOffsetY;
    int mFaceMode;
    int moveCheck(cv::Mat img);
    void irisDetecRun();

signals:
    void readyDone();

protected:
    void run();
};

#endif // READYTHREAD_H