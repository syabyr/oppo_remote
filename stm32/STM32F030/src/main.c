
#include "include.h"

extern volatile unsigned short RXcounter;
extern volatile unsigned char  RXbuffer[256];
unsigned char Version;
SYS Sys;

static void SysTickConfig(void)
{
  SysTick_Config(SystemCoreClock / 100);
  NVIC_SetPriority(SysTick_IRQn, 0x0);
}
void wdg_init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_64);		// 625Hz
	IWDG_SetReload(1250);										// 2Second RST
	IWDG_ReloadCounter();
  IWDG_Enable();
}
void rf24g(void * arg)
{
	u8 buf[5],len;
	unsigned char i;
	if(!Sys.have24g) return;
	len = rf24g_rx(Sys.rx24g.buf);
	if(len)
	{
		if(len<11)
		{
			Sys.rx24g.Bytes.type = 0x01;
			usart_putc(len+3);
			usart_putc(CMD_GET_24G);
			usart_putc(Sys.rx24g.Bytes.type);
			for(i=0;i<len;i++)
			{
				usart_putc(Sys.rx24g.buf[i]);
			}
			_delay_ms(20);
		}
		else
		{
			Sys.rx24g.Bytes.type = 0x02;
			usart_putc(len+3);
			usart_putc(CMD_GET_24G);
			usart_putc(Sys.rx24g.Bytes.type);
			for(i=0;i<len;i++)
			{
				usart_putc(Sys.rx24g.buf[i]);
			}
			_delay_ms(20);
			if(Sys.rx24g.Bytes.cmd == RF_CMD_FactTest)
			{
				rfask_int_disable();
				//lt8900_init();
				buf[0] = 1;
				buf[1] = 2;
				buf[2] = 3;
				buf[3] = 4;
				buf[4] = RF_CMD_FactTestRsp;
				rf24g_tx(2,buf,8,10);
				rfask_int_enable();
			}
		}
	}
}
void rf433(void * arg)
{
	unsigned char i;
	u32 tempp = 0;
	if(Sys.rf433.rxok)
	{
		Sys.rf433.rxok = 0;
		usart_putc(Sys.rf433.rxbits*2+4+3);
		usart_putc(CMD_GET_433);
		usart_putc(0x02);
		for(i=0;i<(Sys.rf433.rxbits*2+4);i++)
		{
			usart_putc(Sys.rf433.rxbuf[i]);
		}
		_delay_ms(20);
		for (i = 0; i < 24; i++)
		{
			tempp <<= 1;
			if (Sys.rf433.rxbuf[i * 2 + 4] > Sys.rf433.rxbuf[i * 2 + 5])
			{
				tempp += 1;
			}
		}
		Sys.rf433.rxbuf[0] = *((u8*) &tempp + 2);
		Sys.rf433.rxbuf[1] = *((u8*) &tempp + 1);
		Sys.rf433.rxbuf[2] = *((u8*) &tempp + 0);
		if(Sys.rf433.rxbuf[0]=='h' && Sys.rf433.rxbuf[1]=='s' && Sys.rf433.rxbuf[2]=='d')
		{
			_delay_ms(500);
			Sys.rf433.txbuf[0]='d';
			Sys.rf433.txbuf[1]='s';
			Sys.rf433.txbuf[2]='h';
			rf433_hs_trasmit(3,6);
		}
	}
}
void rf315(void * arg)
{
	unsigned char i;
	u32 tempp = 0;
	if(Sys.rf315.rxok)
	{
		Sys.rf315.rxok = 0;
		usart_putc(Sys.rf315.rxbits*2+4+3);
		usart_putc(CMD_GET_315);
		usart_putc(0x02);
		for(i=0;i<(Sys.rf315.rxbits*2+4);i++)
		{
			usart_putc(Sys.rf315.rxbuf[i]);
		}
		_delay_ms(20);
		for (i = 0; i < 24; i++)
		{
			tempp <<= 1;
			if (Sys.rf315.rxbuf[i * 2 + 4] > Sys.rf315.rxbuf[i * 2 + 5])
			{
				tempp += 1;
			}
		}
		Sys.rf315.rxbuf[0] = *((u8*) &tempp + 2);
		Sys.rf315.rxbuf[1] = *((u8*) &tempp + 1);
		Sys.rf315.rxbuf[2] = *((u8*) &tempp + 0);
		if(Sys.rf315.rxbuf[0]=='h' && Sys.rf315.rxbuf[1]=='s' && Sys.rf315.rxbuf[2]=='d')
		{
			_delay_ms(500);
			Sys.rf315.txbuf[0]='d';
			Sys.rf315.txbuf[1]='s';
			Sys.rf315.txbuf[2]='h';
			rf315_hs_trasmit(3,6);
		}
	}
}
void usart_rx(unsigned char *buf,unsigned short len)
{
	u8 outbuf[64];
	unsigned char cmd;
	u8 paralen = 0;
	if (len != buf[0])
	{
		os_printf("usart_rx:wrong length:%d<->%d\r\n",len,buf[0]);
		return;
	}
	cmd = buf[1];
	if(cmd == CMD_GET_VERSION)
	{
		usart_putc(0x03);
		usart_putc(CMD_REPORT_VERSION);
		usart_putc(Version);
		_delay_ms(20);
	}
	else if(cmd == CMD_SEND_24G)
	{
		if(len > 13)
		{
			paralen = len - 13;
		}
		memcpy(outbuf, &buf[3], 9);
		if(paralen > 80)
		{
			
		}
		else if(paralen > 40)
		{
			outbuf[9] = 0x01;
			memcpy(&outbuf[10],&buf[12],40);
			rf24g_tx(buf[2],outbuf,len,10);
		}
		else
		{
			rf24g_tx(buf[2],&buf[3],len,10);
		}
		usart_putc(0x03);
		usart_putc(CMD_ACK);
		usart_putc(cmd);
		_delay_ms(20);
	}
	else if(cmd == CMD_SEND_433)
	{
		if(buf[2] == 0x01)
		{
			Sys.rf433.txbuf[0]=buf[3];
			Sys.rf433.txbuf[1]=buf[4];
			Sys.rf433.txbuf[2]=buf[5];
			rf433_hs_trasmit(3,8);
		}
		else if(buf[2] == 0x02)
		{
			rf433_transmit(&buf[3],len-3,10);
		}
		usart_putc(0x03);
		usart_putc(CMD_ACK);
		usart_putc(cmd);
		_delay_ms(20);
	}
	else if(cmd == CMD_SEND_315)
	{
		if(buf[2] == 0x01)
		{
			Sys.rf315.txbuf[0]=buf[3];
			Sys.rf315.txbuf[1]=buf[4];
			Sys.rf315.txbuf[2]=buf[5];
			rf315_hs_trasmit(3,8);
		}
		else if(buf[2] == 0x02)
		{
			rf315_transmit(&buf[3],len-3,10);
		}
		usart_putc(0x03);
		usart_putc(CMD_ACK);
		usart_putc(cmd);
		_delay_ms(20);
	}
	else if(cmd == CMD_TEST_315)
	{
		usart_putc(0x03);
		usart_putc(CMD_ACK);
		usart_putc(cmd);
		_delay_ms(20);
		Sys.rftest = 1;
	}
	else if(cmd == CMD_TEST_433)
	{
		usart_putc(0x03);
		usart_putc(CMD_ACK);
		usart_putc(cmd);
		_delay_ms(20);
		Sys.rftest = 2;
	}
	else if(cmd == CMD_TEST_24G)
	{
		usart_putc(0x03);
		usart_putc(CMD_ACK);
		usart_putc(cmd);
		_delay_ms(20);
		RFX2401_TXEN_HIGH();
		LT_WriteReg(7,0x0000);
		LT_WriteReg(32,0x1807);
		LT_WriteReg(34,0x830b);
		LT_WriteReg(11,0x8008);
		LT_WriteReg(7,0x014a);
		while(1)
		{
			IWDG_ReloadCounter();
		}
	}
	else
	{
		os_printf("usart_rx:NG->unknown cmd\r\n");
	}
	rfask_int_enable();
}
void timer16_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	TIM_PrescalerConfig(TIM16, 47, TIM_PSCReloadMode_Immediate);
	TIM_SetCounter(TIM16, 0);
	TIM_Cmd(TIM16, ENABLE);
}
int main(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	Version = 0x02;
	usart_init(115200);
	os_printf("i am mcu app : %d.%d\r\n",Version&0xf0,Version&0x0f);
	timer16_init();
	if(lt8900_init())
	{
		Sys.have24g = 1;
		os_printf("SELF-CHECK:OK->lt8900 init\r\n");
	}
	else
	{
		Sys.have24g = 0;
		os_printf("SELF-CHECK:NG->lt8900 init\r\n");
	}
	rfask_init();
	wdg_init();
	SysTickConfig();
	OS_Init_Task();
	OS_Add_Task(ID_USART	, usart  		, 0	, 0	, OS_TICKS_PER_TIK      , 0);
	OS_Add_Task(ID_RF24G	, rf24g  		, 0	, 0	, OS_TICKS_PER_TIK      , 0);
	OS_Add_Task(ID_RF433	, rf433  		, 0	, 0	, OS_TICKS_PER_TIK      , 0);
	OS_Add_Task(ID_RF315	, rf315  		, 0	, 0	, OS_TICKS_PER_TIK      , 0);
	while(1)
	{
		IWDG_ReloadCounter();
		OS_Dispatch_Tasks();
		if(Sys.rftest == 1)
		{
			rf315_hs_trasmit(3,8);
		}
		else if(Sys.rftest == 2)
		{
			rf433_hs_trasmit(3,8);
		}
		else if(Sys.rftest == 3)
		{
			rfask_int_disable();
			rf24g_tx(2,Sys.rx24g.buf,8,10);
			rfask_int_enable();
		}
	}
}






#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  while (1)
  {
  }
}
#endif
