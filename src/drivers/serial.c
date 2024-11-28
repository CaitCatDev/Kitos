#include <stdint.h>

#include <drivers/serial.h>

#include <pio.h>

int init_serial(uint16_t port) {
	out8(port + 1, 0x00);
	out8(port + 3, 0x80);
	
	/*Set timer*/
	out8(port + 0, 0x03);
	out8(port + 1, 0x00);

	out8(port + 3, 3);
	
	/*FIFO*/
	out8(port + 2, 0xc7);
	out8(port + 4, 0x0b);
	out8(port + 4, 0x1e);

	/*Loopback*/
	out8(port, 0xca);

	if(in8(port) != 0xca) {
		return -1;
	}

	out8(port + 4, 0x0f);
	return 0;
}

int is_transmit_empty(uint16_t port) {
	return in8(port + 5) & 0x20;
}

void write_serial(uint8_t a) {
	while (is_transmit_empty(COM1) == 0);
	
	if(a == '\n') {
		out8(COM1, '\r');
		while (is_transmit_empty(COM1) == 0);
	}
	out8(COM1,a);
}

