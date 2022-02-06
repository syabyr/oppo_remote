
#include "include.h"

void _delay_us(unsigned int t)
{
	unsigned int i=0;      
	while(t--)    
	{        
		i=11;
		while(i--);
	}
}
void _delay_ms(unsigned int t)
{
	unsigned int i=0;      
	while(t--)
	{        
		IWDG_ReloadCounter();
		i=12000;
		while(i--);
	} 
}
void _delay_us_timer(u16 t)
{
	TIM_SetCounter(TIM16, 0);
	while(TIM_GetCounter(TIM16) < t);
}
