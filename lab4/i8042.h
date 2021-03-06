#ifndef __I8042
#define __I8042

#define BIT(n) (0x01<<(n))

#define ESC  0x81 //Escape key break code

#define STAT_REG 0x64 //status register

#define KBC_CMD_REG 0x64 //keyboard command register to read the kbc state

#define OUT_BUF 0x60 //output buffer

#define KEYBOARD_IRQ	1  // copy

#define IN_BUF		0x60  //input buffer

#define IBF			BIT(1) //bit 1

#define OBF			BIT(0) // 0x01

#define PAR_ERR     BIT(7) // parity bit

#define TO_ERR	    BIT(6) // timed out bit

#define WAIT_KBC    20000 //used in delay

#define READ_BYTE 	0x20 //read byte

#define WRITE_BYTE  0x60 //write byte

#define MOUSE_IRQ 12 // copy

#define MOUSE_SEND_COMMAND 0xD4

#define MOD_STREAM 0xEA

#define ACK 0xFA

#define ENABLE 0xF4

#define DISABLE 0xF5

#define ENABLE 0xF4 //enable data reporting
#define DISABLE 0xF5 //disable data reporting

#define WAIT_KBC    20000 //used in delay

#define MOUSE_SYNC BIT(3)

#define ENABLE_MOUSE 0xA8



#endif
