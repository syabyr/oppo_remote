
#include "include.h"

extern volatile unsigned short RXcounter;
extern volatile unsigned char  RXbuffer[256];

unsigned char rf433_bits,rf315_bits;
u16 CaptureTime433,CaptureTime315,H433,H315;
unsigned char rf433_data[RF_MAX_BITS*2+4];
unsigned char rf315_data[RF_MAX_BITS*2+4];

void NMI_Handler(void)
{
}
void HardFault_Handler(void)
{
  while (1)
  {
  }
}
void SVC_Handler(void)
{
}
void PendSV_Handler(void)
{
}
void SysTick_Handler(void)
{
	OS_Update_Task();
}
void USART1_IRQHandler(void)
{
	rfask_int_disable();
  if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
  {
		usart_rx_lock = 1;
		RXbuffer[RXcounter++] = USART_ReceiveData(USART1);
  }
}
void TIM1_CC_IRQHandler(void)
{
	u8 i,absv;
  if(TIM_GetITStatus(TIM1, TIM_IT_CC4) == SET)
  {
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC4);
		CaptureTime433 = TIM_GetCapture4(TIM1) / 10; 
		TIM_SetCounter(TIM1,0);
		if (!Sys.rf433.rxok)
		{
			if (GPIO_ReadInputDataBit(RF433_IN_GPIO,RF433_IN_PIN))
			{
				if (CaptureTime433 > RF_LEAD_VAL)
				{
					if (rf433_bits >= 24 && rf433_bits <= RF_MAX_BITS)
					{
						rf433_data[0] = ((H433>>8)&0xff);
						rf433_data[1] = ((H433>>0)&0xff);						
						rf433_data[2] = ((CaptureTime433>>8)&0xff);
						rf433_data[3] = ((CaptureTime433>>0)&0xff);
						for(i=4;i<(rf433_bits*2+4);i++)
						{
							absv = fabs(rf433_data[i]-Sys.rf433.rxbuf[i]);
							if((absv*2) > rf433_data[i])
							{
								break;
							}
						}
						memcpy(Sys.rf433.rxbuf, rf433_data, sizeof(rf433_data));
						if(i >= (rf433_bits*2+4))
						{
							Sys.rf433.rxbits = rf433_bits;
							Sys.rf433.rxok = 1;
						}
					}
					rf433_bits = 0;
					memset(rf433_data, 0, sizeof(rf433_data));
				}
				else if (CaptureTime433 > RF_BIT0_VAL) // bit0
				{
					rf433_data[rf433_bits*2+5] = CaptureTime433;
					rf433_bits++;
				}
				else
				{
					rf433_bits = 0;
					memset(rf433_data, 0, sizeof(rf433_data));
				}
			}
			else
			{
				rf433_data[rf433_bits*2+4] = CaptureTime433;
				H433 = CaptureTime433;
			}
		}
  }
}
void TIM3_IRQHandler(void)
{
	u8 i,absv;
	if(TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
		CaptureTime315 = TIM_GetCapture2(TIM3) / 10; 
		TIM_SetCounter(TIM3,0);
		if (!Sys.rf315.rxok)
		{
			if (GPIO_ReadInputDataBit(RF315_IN_GPIO,RF315_IN_PIN))
			{
				if (CaptureTime315 > RF_LEAD_VAL)
				{
					if (rf315_bits >= 24 && rf315_bits <= RF_MAX_BITS)
					{
						rf315_data[0] = ((H315>>8)&0xff);
						rf315_data[1] = ((H315>>0)&0xff);						
						rf315_data[2] = ((CaptureTime315>>8)&0xff);
						rf315_data[3] = ((CaptureTime315>>0)&0xff);
						for(i=4;i<(rf315_bits*2+4);i++)
						{
							absv = fabs(rf315_data[i]-Sys.rf315.rxbuf[i]);
							if((absv*2) > rf315_data[i])
							{
								break;
							}
						}
						memcpy(Sys.rf315.rxbuf, rf315_data, sizeof(rf315_data));
						if(i >= (rf315_bits*2+4))
						{
							Sys.rf315.rxbits = rf315_bits;
							Sys.rf315.rxok = 1;
						}
					}
					rf315_bits = 0;
					memset(rf315_data, 0, sizeof(rf315_data));
				}
				else if (CaptureTime315 > RF_BIT0_VAL) // bit0
				{
					rf315_data[rf315_bits*2+5] = CaptureTime315;
					rf315_bits++;
				}
				else
				{
					rf315_bits = 0;
					memset(rf315_data, 0, sizeof(rf315_data));
				}
			}
			else
			{
				rf315_data[rf315_bits*2+4] = CaptureTime315;
				H315 = CaptureTime315;
			}
		}
	}
}
