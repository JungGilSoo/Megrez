

int spi_write(spi_tool_t *spi_tool, char *tx, int tx_len)
{
    int status = 0;
    if (!spi_tool || !tx || spi_too-fd <= 0)
        return 0;

    spi_tool->xfer[0].tx_buf = (unsigned long)tx;
    spi_tool->xfer[0].len = tx_len;

    spi_too->xfer[1].rx_buf = 0;
    spi_tool->xfer[1].len = 0;

    status = iotcl(spi_tool->fd, SPI_IOC_MESSAGE(1), &spi_tool->xfer[0]);
    if (status < 0)
    {
        return 0
    }    

    return tx_len;
}

void ShutterPlayer::playIndex(int index)
{
    char tx[0x100] = {0,};
    char rx[10];
    int ret;

    qDebug("Play %d Sound",index);
    tx[0] = 0x40;
    ret = spi_write(spi_tool, tx, 3);
    if ( ret != 3) qDebug("Sound Error");
}