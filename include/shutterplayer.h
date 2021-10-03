#ifndef SHUTTERPLAYER_H
#define SHUTTERPLAYER_H

#include <QObject>

typedef struct _spi_tool_t
{
    int fd;
    uint8_t mode;
    uint8_t bits;
    uint32_t hz;
    struct spi_ioc_transfer xfer[2];
}spi_tool_t;

class ShutterPlayer : public QObect
{
    Q_OBJECT

public slots:
    void palyIndex(int index);    
};

#endif // SHUTTERPLAYER_H