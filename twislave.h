#ifndef _TWI_SLAVE_H_
#define _TWI_SLAVE_H_

extern unsigned char I2C_RXBuffer;
extern unsigned char Byte_Counter;

extern void InitIC2_Slave (uint8_t adr);

#define TWI_BUS_ERR_1   0x00
#define TWI_BUS_ERR_2   0xF8

// Status Slave RX Mode
#define SR_SLA_ACK      0x60
#define SR_LOST_ACK     0x68
#define SR_GEN_CALL_ACK 0x70
#define GEN_LOST_ACK    0x78
#define SR_PREV_ACK     0x80
#define SR_PREV_NACK    0x88
#define GEN_PREV_ACK    0x90
#define GEN_PREV_NACK   0x98
#define STOP_CONDITION  0xA0
#define REPEATED_START  0xA0

// Status Slave TX mode
#define SW_SLA_ACK      0xA8
#define SW_LOST_ACK     0xB0
#define SW_DATA_ACK     0xB8
#define SW_DATA_NACK    0xC0
#define SW_LAST_ACK     0xC8

#endif

