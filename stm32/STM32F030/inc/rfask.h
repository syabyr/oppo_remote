
#ifndef	_RFASK_H_
#define	_RFASK_H_




void rfask_init(void);
void rf433_transmit(u8 *buf,u8 len,u8 rept);
void rf315_transmit(u8 *buf,u8 len,u8 rept);
void rf433_hs_trasmit(unsigned char len, unsigned char repeat);
void rf315_hs_trasmit(unsigned char len, unsigned char repeat);
void rfask_int_enable(void);
void rfask_int_disable(void);

#endif
