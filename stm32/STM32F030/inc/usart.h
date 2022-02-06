
#ifndef	_USART_H_
#define	_USART_H_






void usart_init(unsigned int baud);
void os_printf(char *fmt, ...);
void usart_putc(unsigned char c);
void usart(void * arg);





#endif
