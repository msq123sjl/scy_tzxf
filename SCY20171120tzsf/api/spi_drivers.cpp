#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include "api/myhelper.h"
#include <stdio.h>
#include <QRegExp>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
float ad_value[8];

static void pabort(const char *s)
{
    perror(s);
    abort();
}
static const char *device = "/dev/spidev2.0";
static u_int8_t mode = 1;
static u_int8_t bits = 8;
static u_int32_t speed = 10000;
//static u_int16_t delay;

int spi_fd;

void SPI_Init()
{
    int ret=0;

    spi_fd = open(device, O_RDWR);
    if (spi_fd < 0)
        pabort("can't open device");
    /*
     * spi mode
     */
    ret = ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        pabort("can't set spi mode");

    ret = ioctl(spi_fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        pabort("can't get spi mode");

    /*
     * bits per word
     */
    ret = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't set bits per word");

    ret = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't get bits per word");

    /*
     * max speed hz
     */
    ret = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't set max speed hz");

    ret = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't get max speed hz");

    printf("spi mode: %d\n", mode);
    printf("bits per word: %d\n", bits);
    printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

}


int spi_read_ad()
{
    char rd_buf[120]={"0"};
    char wr_buf[]={"##FC=1;CRC=6740\r\n"};
    QRegExp  Ex;
    int pos;

    if( write(spi_fd, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf)){
        perror("Write Error");
        return false;
    }
    sleep(1);
    if( read(spi_fd, rd_buf, ARRAY_SIZE(rd_buf)) != ARRAY_SIZE(rd_buf)){
        perror("Read Error");
        return false;
    }else{
        if(rd_buf[0]=='\0'&&rd_buf[1]=='\0')
        {
            printf("Read Empty\n");
            return false;
        }

        Ex.setPattern("U0=([0-9|.]+)");
        pos=Ex.indexIn(rd_buf);
        if(pos != -1){
            ad_value[0]=Ex.cap(1).toFloat();
        }
        else {
            ad_value[0]=0;
        }

        Ex.setPattern("U1=([0-9|.]+)");
        pos=Ex.indexIn(rd_buf);
        if(pos != -1){
            ad_value[1]=Ex.cap(1).toFloat();
        }
        else {
            ad_value[1]=0;
        }

        Ex.setPattern("U2=([0-9|.]+)");
        pos=Ex.indexIn(rd_buf);
        if(pos != -1){
            ad_value[2]=Ex.cap(1).toFloat();
        }
        else {
            ad_value[2]=0;
        }

        Ex.setPattern("U3=([0-9|.]+)");
        pos=Ex.indexIn(rd_buf);
        if(pos != -1){
            ad_value[3]=Ex.cap(1).toFloat();
        }
        else {
            ad_value[3]=0;
        }

        Ex.setPattern("U4=([0-9|.]+)");
        pos=Ex.indexIn(rd_buf);
        if(pos != -1){
            ad_value[4]=Ex.cap(1).toFloat();
        }
        else {
            ad_value[4]=0;
        }

        Ex.setPattern("U5=([0-9|.]+)");
        pos=Ex.indexIn(rd_buf);
        if(pos != -1){
            ad_value[5]=Ex.cap(1).toFloat();
        }
        else {
            ad_value[5]=0;
        }

        Ex.setPattern("U6=([0-9|.]+)");
        pos=Ex.indexIn(rd_buf);
        if(pos != -1){
            ad_value[6]=Ex.cap(1).toFloat();
        }
        else {
            ad_value[6]=0;
        }

        Ex.setPattern("U7=([0-9|.]+)");
        pos=Ex.indexIn(rd_buf);
        if(pos != -1){
            ad_value[7]=Ex.cap(1).toFloat();
        }
        else {
            ad_value[7]=0;
        }
    }

    return true;

}


