#ifndef IRISDEVICE_H
#define IRISDEVICE_H

#include <QObject>

class IrisDevice : public QObject
{
    Q_OBJECT

public:
    explicit IrisDevice(QObject *parent = nullptr);
    ~IrisDevice();

    bool initialize();
    bool isInitialize();

    int getCamID();
    int getCamID1();
    int getWebCamID();
    int mUsbType;

private:
    bool m_initialize;
    int m_camID;
    int m_camID1;
    int m_webcamID;
    int countCameras(int num);

signals:
    void connected(const QString&);            
};

#endif // IRISDEVICE_H