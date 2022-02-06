
#include "include.h"

#define RF433_OUT_LOW()     GPIO_ResetBits(RF433_OUT_GPIO, RF433_OUT_PIN)
#define RF433_OUT_HIGH()    GPIO_SetBits(RF433_OUT_GPIO, RF433_OUT_PIN)
#define RF315_OUT_LOW()     GPIO_ResetBits(RF315_OUT_GPIO, RF315_OUT_PIN)
#define RF315_OUT_HIGH()    GPIO_SetBits(RF315_OUT_GPIO, RF315_OUT_PIN)

void rfask_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
//	EXTI_InitTypeDef   EXTI_InitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	// 433OUT
	GPIO_InitStructure.GPIO_Pin = RF433_OUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(RF433_OUT_GPIO, &GPIO_InitStructure);
	// 315OUT
	GPIO_InitStructure.GPIO_Pin = RF315_OUT_PIN;
	GPIO_Init(RF315_OUT_GPIO, &GPIO_InitStructure);
	
	RF433_OUT_LOW();
	RF315_OUT_LOW();
	// 433IN
	GPIO_InitStructure.GPIO_Pin =  RF433_IN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(RF433_IN_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(RF433_IN_GPIO, RF433_IN_SOURCE, GPIO_AF_2);
	// 315IN
	GPIO_InitStructure.GPIO_Pin =  RF315_IN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(RF315_IN_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(RF315_IN_GPIO, RF315_IN_SOURCE, GPIO_AF_1);
	// 433 timer
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_PrescalerConfig(TIM1, 47, TIM_PSCReloadMode_Immediate);
	TIM_SetCounter(TIM1, 0);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM1, &TIM_ICInitStructure);
	TIM_Cmd(TIM1, ENABLE);
	TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	// 315 timer
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_PrescalerConfig(TIM3, 47, TIM_PSCReloadMode_Immediate);
	TIM_SetCounter(TIM3, 0);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void rfask_int_enable(void)
{
	TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
}
void rfask_int_disable(void)
{
	TIM_ITConfig(TIM1, TIM_IT_CC4, DISABLE);
	TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
}
void rf433_transmit(u8 *buf,u8 len,u8 rept)
{
	u8 i;
	u16 temp;
	while(rept--)
	{
		temp  = buf[0];
		temp<<= 8;
		temp += buf[1];
		RF433_OUT_HIGH();
		_delay_us_timer(temp*10);
		temp  = buf[2];
		temp<<= 8;
		temp += buf[3];
		RF433_OUT_LOW();
		_delay_us_timer(temp*10);
		for(i=4;i<len;i+=2)
		{
			RF433_OUT_HIGH();
			_delay_us_timer(buf[i]*10);
			RF433_OUT_LOW();
			_delay_us_timer(buf[i+1]*10);
		}
	}
}
void rf315_transmit(u8 *buf,u8 len,u8 rept)
{
	u8 i;
	u16 temp;
	while(rept--)
	{
		temp  = buf[0];
		temp<<= 8;
		temp += buf[1];
		RF315_OUT_HIGH();
		_delay_us_timer(temp*10);
		temp  = buf[2];
		temp<<= 8;
		temp += buf[3];
		RF315_OUT_LOW();
		_delay_us_timer(temp*10);
		for(i=4;i<len;i+=2)
		{
			RF315_OUT_HIGH();
			_delay_us_timer(buf[i]*10);
			RF315_OUT_LOW();
			_delay_us_timer(buf[i+1]*10);
		}
	}
}
void rf433_hs_trasmit(unsigned char len, unsigned char repeat)
{
	unsigned char i, j;
	unsigned char buf[10];
	if (len > 10 || len == 0) return;
	rfask_int_disable();
	while (repeat--)
	{
		memcpy(buf, Sys.rf433.txbuf, sizeof (buf));
		RF433_OUT_HIGH();
		_delay_us(RF_HS_4CLK);
		RF433_OUT_LOW();
		_delay_ms(9);
		for (i = 0; i < len; i++)
		{
			for (j = 0; j < 8; j++)
			{
				if (buf[i] & 0x80)
				{
					RF433_OUT_HIGH();
					_delay_us(RF_HS_4CLK * 3);
					RF433_OUT_LOW();
					_delay_us(RF_HS_4CLK);
				}
				else
				{
					RF433_OUT_HIGH();
					_delay_us(RF_HS_4CLK);
					RF433_OUT_LOW();
					_delay_us(RF_HS_4CLK * 3);
				}
				buf[i] <<= 1;
			}
		}
	}
	rfask_int_enable();
}
void rf315_hs_trasmit(unsigned char len, unsigned char repeat)
{
	unsigned char i, j;
	unsigned char buf[10];
	if (len > 10 || len == 0) return;
	rfask_int_disable();
	while (repeat--)
	{
		memcpy(buf, Sys.rf315.txbuf, sizeof (buf));
		RF315_OUT_HIGH();
		_delay_us(RF_HS_4CLK);
		RF315_OUT_LOW();
		_delay_ms(9);
		for (i = 0; i < len; i++)
		{
			for (j = 0; j < 8; j++)
			{
				if (buf[i] & 0x80)
				{
					RF315_OUT_HIGH();
					_delay_us(RF_HS_4CLK * 3);
					RF315_OUT_LOW();
					_delay_us(RF_HS_4CLK);
				}
				else
				{
					RF315_OUT_HIGH();
					_delay_us(RF_HS_4CLK);
					RF315_OUT_LOW();
					_delay_us(RF_HS_4CLK * 3);
				}
				buf[i] <<= 1;
			}
		}
	}
	rfask_int_enable();
}

