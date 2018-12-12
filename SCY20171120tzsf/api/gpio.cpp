#include "api/gpio.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


int fd;
int Power_Change=0;
int Power_Old=0;
int Power_New=0;


int GPIO_OutEnable(int fd, unsigned int dwEnBits)
{
    int 				rc;
    struct double_pars	dpars;

    dpars.par1 = EM335X_GPIO_OUTPUT_ENABLE;		// 0
    dpars.par2 = dwEnBits;

    rc = write(fd, &dpars, sizeof(struct double_pars));
    return rc;
}

int GPIO_OutDisable(int fd, unsigned int dwDisBits)
{
    int 				rc;
    struct double_pars	dpars;

    dpars.par1 = EM335X_GPIO_OUTPUT_DISABLE;	// 1
    dpars.par2 = dwDisBits;

    rc = write(fd, &dpars, sizeof(struct double_pars));
    return rc;
}

int GPIO_OutSet(int fd, unsigned int dwSetBits)
{
    int 				rc;
    struct double_pars	dpars;

    dpars.par1 = EM335X_GPIO_OUTPUT_SET;	// 2
    dpars.par2 = dwSetBits;

    rc = write(fd, &dpars, sizeof(struct double_pars));
    return rc;
}

int GPIO_OutClear(int fd, unsigned int dwClearBits)
{
    int 				rc;
    struct double_pars	dpars;

    dpars.par1 = EM335X_GPIO_OUTPUT_CLEAR;	// 3
    dpars.par2 = dwClearBits;

    rc = write(fd, &dpars, sizeof(struct double_pars));
    return rc;
}

int GPIO_PinState(int fd, unsigned int* pPinState)
{
    int 				rc;
    struct double_pars	dpars;

    dpars.par1 = EM335X_GPIO_INPUT_STATE;	// 5
    dpars.par2 = *pPinState;

    rc = read(fd, &dpars, sizeof(struct double_pars));
    if(!rc)
    {
        *pPinState = dpars.par2;
    }
    return rc;
}

int InitGPIO()
{
    int rc;
    fd=open("/dev/em335x_gpio",O_RDWR);
    printf("open file = %d\n", fd);
    rc = GPIO_OutEnable(fd,GPIO0|SWITCH_OUT1|SWITCH_OUT2|SWITCH_OUT3|SWITCH_OUT4|SWITCH_OUT5|SWITCH_OUT6|SWITCH_OUT7|SWITCH_OUT8|SPI_CS);//set GPIO as output
    if(rc < 0)
    {
        printf("GPIO_OutEnable::failed %d\n", rc);
    }

    rc = GPIO_OutDisable(fd,SWITCH_IN1|SWITCH_IN2|SWITCH_IN3|SWITCH_IN4|SWITCH_IN5|SWITCH_IN6|SWITCH_IN7|SWITCH_IN8|SWITCH_IN9|SWITCH_IN10|SWITCH_IN11|SWITCH_IN12);   //set GPIO as input
    if(rc < 0)
    {
        printf("GPIO_OutClear::failed %d\n", rc);
    }
    usleep(10000);

    GPIO_OutDisable(fd,1<<31);
    if(rc>0)close(rc);
    return rc;

}

void SwitchOut_On(int ch)
{
    GPIO_OutClear(fd,1<<ch);
}

void SwitchOut_Off(int ch)
{
    GPIO_OutSet(fd,1<<ch);
}

bool GetSwitchStatus(int ch)
{
    unsigned int dwPinState=0xFFFFFFFF;
    bool status;
    GPIO_PinState(fd, &dwPinState); //read states of all bits

    if(((dwPinState>>ch)&0x1) == 0) {
        status=true;
    }else {
        status=false;
    }

    return status;
}

bool wdt_sta=true;
bool WDT_Feed()
{
    if(wdt_sta==true)
    {
        GPIO_OutClear(fd,GPIO0);
    }
    else
    {
        GPIO_OutSet(fd,GPIO0);
    }
    wdt_sta=!wdt_sta;

    return wdt_sta;
}

