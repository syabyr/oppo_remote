
#ifndef	_DEFINE_H_
#define	_DEFINE_H_

typedef unsigned char 			u8;
typedef unsigned short 			u16;
typedef unsigned long 			u32;

#define	RF433_IN_GPIO				GPIOA
#define	RF433_IN_PIN				GPIO_Pin_11
#define	RF433_IN_SOURCE			GPIO_PinSource11
#define	RF433_OUT_GPIO			GPIOB
#define	RF433_OUT_PIN				GPIO_Pin_4
#define	RF315_IN_GPIO				GPIOB
#define	RF315_IN_PIN				GPIO_Pin_5
#define	RF315_IN_SOURCE			GPIO_PinSource5
#define	RF315_OUT_GPIO			GPIOA
#define	RF315_OUT_PIN				GPIO_Pin_12

#define	LT8900_SDN_GPIO			GPIOA
#define	LT8900_SDN_PIN			GPIO_Pin_4
#define	LT8900_SCK_GPIO			GPIOA
#define	LT8900_SCK_PIN			GPIO_Pin_5
#define LT8900_SCK_SOURCE 	GPIO_PinSource5
#define LT8900_SCK_AF     	GPIO_AF_0
#define	LT8900_MISO_GPIO		GPIOA
#define	LT8900_MISO_PIN			GPIO_Pin_6
#define LT8900_MISO_SOURCE 	GPIO_PinSource6
#define LT8900_MISO_AF     	GPIO_AF_0
#define	LT8900_MOSI_GPIO		GPIOA
#define	LT8900_MOSI_PIN			GPIO_Pin_7
#define LT8900_MOSI_SOURCE 	GPIO_PinSource7
#define LT8900_MOSI_AF     	GPIO_AF_0
#define	LT8900_CS_GPIO			GPIOB
#define	LT8900_CS_PIN				GPIO_Pin_0
#define	LT8900_IRQ_GPIO			GPIOB
#define	LT8900_IRQ_PIN			GPIO_Pin_1

#define	RFX2401_RXEN_PIN		GPIO_Pin_2
#define	RFX2401_RXEN_GPIO		GPIOA
#define	RFX2401_TXEN_PIN		GPIO_Pin_3
#define	RFX2401_TXEN_GPIO		GPIOA

#define	USART_TXD_GPIO			GPIOA
#define	USART_TXD_PIN				GPIO_Pin_9
#define USART_TXD_SOURCE 		GPIO_PinSource9
#define USART_TXD_AF     		GPIO_AF_1
#define	USART_RXD_GPIO			GPIOA
#define	USART_RXD_PIN				GPIO_Pin_10
#define USART_RXD_SOURCE 		GPIO_PinSource10
#define USART_RXD_AF     		GPIO_AF_1

#define	RF_CMD_IR_SEND					0x01
#define	RF_CMD_IR_STRUDY				0x02
#define	RF_CMD_IR_REPORT				0x03
#define	RF_CMD_GET_VERSION			0x04
#define	RF_CMD_REPORT_VERSION		0x05
#define	RF_CMD_PAIR							0x06
#define	RF_CMD_FactTest					0x07
#define	RF_CMD_FactTestRsp			0x09


#define	PRODUCT_MASTER					0x01
#define	PRODUCT_REPEATER				0x02

#define CMD_ACK							0x02
#define CMD_SEND_24G				0x03
#define CMD_GET_VERSION			0x04
#define CMD_REPORT_VERSION	0x05
#define CMD_GET_24G					0x06
#define CMD_GET_868					0x07
#define CMD_SEND_868				0x08
#define CMD_GET_433					0x09
#define CMD_SEND_433				0x0a
#define CMD_GET_315					0x0b
#define CMD_SEND_315				0x0c
#define CMD_TEST_315				0x0d
#define CMD_TEST_433				0x0e
#define CMD_TEST_24G				0x0f
#define	CMD_DIS_USART				0xfe

#define	RF_LEAD_VAL			 		200		//  2ms
#define	RF_BIT0_VAL					13		//  130us
#define RF_MAX_BITS					64
#define RF_TX_REPEAT				8
#define RF_HS_4CLK					280

typedef	union 
{
	struct 
	{
		u8 len;
		u8 enc;
		u8 roll;
		u8 id[4];
		u8 type;
		u8 rept;
		u8 reptt;
		u8 cmd;
		u8 para[53];
	}Bytes;
	u8 buf[64];
} RF24G;
typedef	union 
{
	struct 
	{
		u8 len;
		u8 enc;
		u8 roll;
		u8 id[4];
		u8 type;
		u8 rept;
		u8 reptt;
		u8 cmd;
		u8 para[53];
	}Bytes;
	u8 buf[64];
} RF868;
typedef struct
{
	unsigned char rxbuf[RF_MAX_BITS*2+4];
	unsigned char txbuf[RF_MAX_BITS*2+4];
	unsigned char rxbits;
	unsigned char rxok;
} RFASK;
typedef struct
{
	RF24G rx24g;
	RF24G tx24g;
	RF868 rf868;
	RFASK rf433;
	RFASK rf315;
	u8 have24g;
	u8 rftest;
}SYS;


#endif
