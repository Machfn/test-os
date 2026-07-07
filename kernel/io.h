#ifndef IO_H_
#define IO_H_

#include "stdint.h"

/** The I/O Ports */
#define FB_COMMAND_PORT 0x3d4
#define FB_DATA_PORT 0x3d5

/** The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 0x0e
#define FB_LOW_BYTE_COMMAND 0x0f

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);
uint8_t scan(void);

void move_cursor(int row, int col);
void printprompt(void);

#endif
