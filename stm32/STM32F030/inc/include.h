
#ifndef	_INCLUDE_H_
#define	_INCLUDE_H_

#include "stm32f0xx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "define.h"
#include "delay.h"
#include "lt8900.h"
#include "rfask.h"
#include "usart.h"
#include "os_core.h"


//extern char sprintfbuf[256];
//extern unsigned short sprintflen;
extern unsigned char usart_rx_lock;
extern SYS Sys;

void usart_rx(unsigned char *buf,unsigned short len);

#endif
