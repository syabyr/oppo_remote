
#include "include.h"
#include <stdarg.h>
#include <string.h>

unsigned short RXcounter=0;
unsigned char RXbuffer[256];
unsigned char usart_rx_lock;

void usart_init(unsigned int baud)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_PinAFConfig(USART_TXD_GPIO, USART_TXD_SOURCE, USART_TXD_AF);
	GPIO_PinAFConfig(USART_RXD_GPIO, USART_RXD_SOURCE, USART_RXD_AF);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = USART_TXD_PIN;
  GPIO_Init(USART_TXD_GPIO, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = USART_RXD_PIN;
  GPIO_Init(USART_RXD_GPIO, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = baud;
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_Even;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);
}
void usart_putc(unsigned char c)
{
	USART_SendData(USART1, c);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}
void usart_putbuf(unsigned char *buf,unsigned short len)
{
	unsigned short i;
	for(i=0;i<len;i++)
	{
		usart_putc(buf[i]);
	}
}
void os_printf(char *fmt, ...)
{
	char buf[256];
	unsigned char len;
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof (buf), fmt, args);
	va_end(args);
	len = strlen(buf);
	usart_putc(len+3);
	usart_putc(CMD_DIS_USART);
	usart_putbuf((unsigned char*)buf,strlen(buf));
	usart_putc('\0');
	_delay_ms(20);
}

void usart(void * arg)
{
	if (usart_rx_lock)
	{
		usart_rx_lock = 0;
		return;
	}
	if (RXcounter)
		usart_rx(RXbuffer, RXcounter);
	RXcounter = 0;
	return;
}

