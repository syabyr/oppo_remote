
#include "include.h"

#define	FREQ	0x4a

unsigned char tab[16]={0x00,0x10,0x20,0x50,0x30,0x40,0x60,0x11,0x21,0x12,0x22,0x13,0x23,0x14,0x24};
unsigned int tx_cout=0;
const unsigned short reg_val_default[][2]=
{
	{0, 0x6fe0},
	{1, 0x5681},
	{2, 0x6617},
	{4, 0x9cc9},
	{5, 0x6637},
	{7, 0x004a},
	{8, 0x6C90},
	{9, 0x1840},		// ?set TX power 2dbm
	//{9, 0x4800},		// ?set TX power 6dbm
	//{9, 0x1fc0},
	{10,0x7FFD},
	{11,0x0008},
	{12,0x0000},
	{13,0x48BD},
	{22,0x00ff},
	{23,0x8005},
	{24,0x0067},
	{25,0x1659},
	{26,0x19E0},
	{27,0x1300},
	{28,0x1800},
		
	{32,0x4800},	// pre:3byes sync:32bits Reg39[15:0]&36[15:0]
	{33,0x3fC7},
	{34,0x2000},
	{35,0x0300},	// if auto ack on,auto ack 3 times
	{36,0x050a},
	{37,0x0380},
	{38,0x5A5A},
	{39,0x55aa},
	{40,0x4401},	// allow wrong 0 bits
	{41,0xb800},	// crc:on 1st length auto ack on
	{42,0xFDB0},	// ?wait176us
	{43,0x000F},	// configure scan_rssi

	{44,0x0100},	// 1Mbps
	{45,0x0152},	// 1Mbps
	
	{52,0x8080},	// clear rx tx fifo dpr,don't clear data
};
unsigned char spi_read_write(unsigned char dat)
{
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
	SPI_SendData8(SPI1, dat); 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	return  SPI_ReceiveData8(SPI1); 
}
void LT_WriteReg(unsigned char reg, unsigned short val)
{
	unsigned char H;
	unsigned char L;
	L = val & 0xff;
	H = val >> 8;
	LT8900_CS_LOW();
	_delay_us(1);
	spi_read_write(reg&0x7f);
	spi_read_write(H);
	spi_read_write(L);
	LT8900_CS_HIGH();
	_delay_us(1);
}
unsigned short LT_ReadReg(unsigned char reg)
{
	unsigned char reh,rel;
	unsigned short re;
	LT8900_CS_LOW();
	_delay_us(1);
	spi_read_write(reg|0x80);
	reh = spi_read_write(0xff);
	rel = spi_read_write(0xff);
	LT8900_CS_HIGH();
	_delay_us(1);
	re = reh;
	re = re << 8;
	re = re + rel;
	return re;
}
void lt8900_init_port(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef	SPI_InitStructure;
	// MOSI
	GPIO_InitStructure.GPIO_Pin = LT8900_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(LT8900_MOSI_GPIO, &GPIO_InitStructure);
	// MISO
	GPIO_InitStructure.GPIO_Pin = LT8900_MISO_PIN;
	GPIO_Init(LT8900_MISO_GPIO, &GPIO_InitStructure);
	// SCK
	GPIO_InitStructure.GPIO_Pin = LT8900_SCK_PIN;
	GPIO_Init(LT8900_SCK_GPIO, &GPIO_InitStructure);
	// CS
	GPIO_InitStructure.GPIO_Pin = LT8900_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(LT8900_CS_GPIO, &GPIO_InitStructure);
	// SDN
	GPIO_InitStructure.GPIO_Pin = LT8900_SDN_PIN;
	GPIO_Init(LT8900_SDN_GPIO, &GPIO_InitStructure);
	// RXEN
	GPIO_InitStructure.GPIO_Pin = RFX2401_RXEN_PIN;
	GPIO_Init(RFX2401_RXEN_GPIO, &GPIO_InitStructure);
	// TXEN
	GPIO_InitStructure.GPIO_Pin = RFX2401_TXEN_PIN;
	GPIO_Init(RFX2401_TXEN_GPIO, &GPIO_InitStructure);
	// IRQ
	GPIO_InitStructure.GPIO_Pin = LT8900_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(LT8900_IRQ_GPIO, &GPIO_InitStructure);	

	GPIO_PinAFConfig(LT8900_MOSI_GPIO, LT8900_MOSI_SOURCE, LT8900_MOSI_AF);
	GPIO_PinAFConfig(LT8900_MISO_GPIO, LT8900_MISO_SOURCE, LT8900_MISO_AF); 
	GPIO_PinAFConfig(LT8900_SCK_GPIO, LT8900_SCK_SOURCE, LT8900_SCK_AF);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction =SPI_Direction_2Lines_FullDuplex; 
	SPI_InitStructure.SPI_Mode =SPI_Mode_Master;    
	SPI_InitStructure.SPI_DataSize =SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL =SPI_CPOL_Low;        //CPOL=0
	SPI_InitStructure.SPI_CPHA =SPI_CPHA_2Edge;       //CPHA=0
	SPI_InitStructure.SPI_NSS =SPI_NSS_Soft;     
	SPI_InitStructure.SPI_BaudRatePrescaler =SPI_BaudRatePrescaler_4; // 12MHz 
	SPI_InitStructure.SPI_FirstBit =SPI_FirstBit_MSB;    
	SPI_InitStructure.SPI_CRCPolynomial =7;        //CRC7 
	SPI_Init(SPI1,&SPI_InitStructure);
	SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
	SPI_Cmd(SPI1, ENABLE);
}
void lt8900_set_rx(void)
{
	LT_WriteReg( 7,0x0000);
	LT_WriteReg(52,0x0080);	// clear rx fifo
	LT_WriteReg( 7,0x0080 | FREQ);
	RFX2401_RXEN_HIGH();
	RFX2401_TXEN_LOW();
}
unsigned short lt8900_init(void)
{
	unsigned char i;
	lt8900_init_port();
	LT8900_SDN_LOW();
	_delay_ms(10);
	LT8900_SDN_HIGH();
	LT8900_CS_HIGH();
	_delay_ms(50);
	for(i=0;i<sizeof(reg_val_default)/sizeof(reg_val_default[0]);i++)
	{
		LT_WriteReg(reg_val_default[i][0],reg_val_default[i][1]);
	}
	_delay_ms(2);
	if(LT_ReadReg(0)==reg_val_default[0][1])
	{
		if(LT_ReadReg(1)==reg_val_default[1][1])
		{
			lt8900_set_rx();
			return 1;
		}
	}
	return 0;
}
void kg_encrypt(u8 *src,u8 len)
{
	u8 i;
	u8 des[64];
	des[0] = src[0];
	des[1] = src[1]^0xbd;
	for(i=2;i<len;i++)
	{
		des[i] = src[i]^des[i-1];
	}
	memcpy(src,des,len);
}
void kg_decrypt(u8 *src,u8 len)
{
	u8 i;
	u8 des[64];
	des[0] = src[0];
	des[1] = src[1]^0xbd;
	for(i=2;i<len;i++)
	{
		des[i] = src[i]^src[i-1];
	}
	memcpy(src,des,len);
}
u8 lt8900_rx(u8 *buf)
{
	u16 len;
	u8 i;
	if(LT8900_IRQ)
	{
		len = LT_ReadReg(48);
		if(!(len&0x8000))
		{
			len = LT_ReadReg(52);
			len <<= 2;
			len >>= 10;
			LT8900_CS_LOW();
			_delay_us(1);
			spi_read_write(50|0x80);
			_delay_us(1);
			for(i=0;i<len;i++)
			{
				buf[i] = spi_read_write(0xaa);
			}
			LT8900_CS_HIGH();
			_delay_us(1);
		}
		else
		{
			lt8900_init();
			return 0;
		}
		lt8900_set_rx();
		if(len) 
			return len;
	}
	return 0;
}
u8 rf24g_rx(u8 *buf)
{
	u8 len;
	len = lt8900_rx(buf);
	if(len<11)
	{
		kg_decrypt(buf,len);
	}
	return len;
}
void lt8900_tx(u8 *buf,u8 len,u8 rept)
{
	u8 i;
	if(len==0 && len>64) return;
	while(rept--)
	{
		LT_WriteReg(52,0x8080);
		LT8900_CS_LOW();
		_delay_us(1);
		spi_read_write(50);
		for(i=0;i<len;i++)
		{
			spi_read_write(buf[i]);
		}
		LT8900_CS_HIGH();
		RFX2401_TXEN_HIGH();	
		LT_WriteReg( 7,0x0100 | FREQ);
		TIM_SetCounter(TIM16, 0);
		while(!LT8900_IRQ)
		{
			if(TIM_GetCounter(TIM16) > 60000)
			{
				break;
			}
		}
	}
	lt8900_set_rx();
}
void rf24g_tx(u8 type,u8 *buf,u8 len,u8 rept)
{
	if(type==0x01)
	{
		buf[4] = buf[1];
		buf[5] = buf[2];
		buf[7] = buf[3];
		buf[3] = buf[0];
		buf[6] = tab[buf[7]];
		tx_cout++;
		buf[0] = len;
		buf[1] = tx_cout&0xff;
		buf[2] = tx_cout >> 8;
		kg_encrypt(buf,len+1);
		lt8900_tx(buf,len+1,rept);
	}
	else if(type==0x02)
	{
		tx_cout++;
		len += 3;
		Sys.tx24g.Bytes.len = len-1;
		Sys.tx24g.Bytes.enc = 0;
		Sys.tx24g.Bytes.roll = tx_cout;
		Sys.tx24g.Bytes.id[0] = buf[0];
		Sys.tx24g.Bytes.id[1] = buf[1];
		Sys.tx24g.Bytes.id[2] = buf[2];
		Sys.tx24g.Bytes.id[3] = buf[3];
		Sys.tx24g.Bytes.type = PRODUCT_MASTER;
		Sys.tx24g.Bytes.rept = 0;
		Sys.tx24g.Bytes.reptt = 0;
		Sys.tx24g.Bytes.cmd = buf[4];
		if(len>11)
		{
			memcpy(Sys.tx24g.Bytes.para,&buf[5],len-11);
		}
		lt8900_tx(Sys.tx24g.buf,len,rept);
	}
}
