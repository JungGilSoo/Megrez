#include "irisdevice.h"
#include "mainwindow.h"
#include "libusb-1.0/libusb.h"

#include <QDebug>

IrisDevice::IrisDevice(QObject *parant) : QObject(parent)
{
    m_camID = -1;
    m_camID1 = -1;
    m_webcamID = 0;
}

IrisDevice::~IrisDevice()
{
    m_initialize = false;
}

int IrisDevice::reset()
{
    struct libusb_device handle *handle;
    libusb_device **device;
    ssize_t device_count;
    int error;
    int rc = 0;
    struct libusb_device_descriptor desc;

    qDebug("Reset USB");
    rc = libusb_init(NULL);
    if(rc) {
        qDebug("USB init error");
        return 1;
    }

    device_count = libusb_get_device_list(NULL, &devices);

    if(device_count < 0){
        return 1;
    }

    for(int i=0; i<device_count; i++){
        libusb_get_device_descriptor(devices[i], &desc);
//        qDebug("USB(%d) : %04x:%04x",desc.bDeviceClass,desc.idVendor,desc.idProduct);

        //if( desc.bDeviceClass != LIBUSB_CLASS_VIDEO){ 
        if(desc.bDeviceClass != 239){
            continue;
        }

        error = libusb_open(device[i], &handle);
//  unsigned car manufacturer[200];
//  memset(manufacturer,0,200);
//  libusb_get_string_descriptor_ascii(handle, desc.iProduct,
//  manufacturer,200);
//        qDebug("Reset[%s] : %04x:%04x",manufacturer,desc.idVendor, desc.idProduct);
//        qDebug("Rest[%s] ",manufacturer);

        unsigned char nproduct[200];
        unsigned char nserial[200];
        unsigned char nconfig[200];
        memset(nproduct,0,200);
        memset(nserial,0,200);
        memset(nconfig,0,200);
        libusb_get_string_descriptor_ascii(handle, desc.iProdict, nproduct,200);
        libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, nserial,200);
        libusb_get_string_descriptor_ascii(handle, desc.bNumConfigurations, nconfig,200);
        qDebug("Device %s:%s:%s ",nproduct,nserial,nconfig);

        if(error){
            qDebug("error open: %04x:04x",desc.idVendor, desc.idProduct);
            return 1;
        }

        if(libusb_reset_device(handle)){
            qDebug("Rest failed: %04x:%04c.\n", desc.idVendor, desc.idProduct);
            rc = 1;
        }
        libusb_close(handle);
    }

    libusb_free_device_list(devices, 1);
    libusb_exit(NULL);

    return rc;
}

bool IrisDevice::initialize()
{
    int num = 10;
//    reset();
    int count = countCameras(num);
    if (count == num)
    {
        // There are no carmeras connected
        return false;
    }
    emit connected(" Alkaid ");
    return true;
}

/* There is a problem with unwanted screens. */
int IrisDevice::CountCameras(int num)
{
    return 3;

    QByteArray arrCamera;
    for(int i = 0; i < num; i++)
    {
        cv::VideoCapture carmera;
        carmera.open(i,cv::CAP_GSTREAMER);
        /* CAP_GSTREAMER is an Open Sourece that makes it easy to process
           video streaming based on Linux. */
        bool res = camerea.isOpened();
        int width = camera.get(cv::CAP_PROP_FRAME_WIDTH);
        int height = camera.get(cv::CAP_PROP_FRAME_HEIGHT);
        int count =camera.get(cv::CAP_PROP_FRAME_COUNT);
        if (res)
        {
            qDebug() << "open camera success: " << i;
#ifdef TEST
    if ( width == 1280 && height == 720 ) m_camID = i;
#else
    if ( width == 2608 && height == 1960 ) {
        if ( m_camID >=0 ) m_camID1 = i;
        else m_camID = i;
//        cv::Mat img;
//        camera.read(img);
        qDebug("Alkaid USB3 type");
        mUsbType=3;
    }
    else if ( width == 1288 ) {
        if ( m_camID >=0 ) m_camID1 = i;
        else m_camID = i;
        qDebug("Alkaid USB2 type");
        mUsbType=2;
    }
    /*
    else if ( width == 640 && heigh == 480 ) {
        m_camID = i;
        qDebug("Alkaid USB2 type with 640*480");
        mUsbType=2;
    }
    */
#endif
    if ( width == theMainWindow->mCamWidth &&
        height == theMainWindow->mCamHeight ) {
        m_webcamID = i;
        qDebug("WEBCAM");
    }

    qDebug("CAM[%d] w,h[%d %d] count[%d] ",i,width,height,count);
    arrCamera.append(i)
    carmera.release();
        }
    }
    int numCamera = arrCamera.count();
    qDebug() << "camera found count: " << numCamera;

    return numCamera;
}

bool IrisDevice::isImitialize()
{
    return m_initialize;
}


int IrisDevice::getCamID1()
{
    return m_camID1;
}


int IrisDevice::getCamID()
{
    return m_camID;
}