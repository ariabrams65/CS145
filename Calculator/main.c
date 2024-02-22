#include "tm4c123gh6pm.h"
#include "bits.h"

// RS - PD0
// R/W - PD1
// EN - PD2
// DP0-DP7 --> {PA7, PA6, PA5, PB4, PE5, PE4, PB1, PB0}

void set_RS(unsigned char rs) {
	if (rs >= 1) {
		GPIO_PORTD_DATA_R |= BIT_0;	
	} else {
		GPIO_PORTD_DATA_R &= ~BIT_0;	
	}
}

void set_RW(unsigned char rw) {
	if (rw >= 1) {
		GPIO_PORTD_DATA_R |= BIT_1;	
	} else {
		GPIO_PORTD_DATA_R &= ~BIT_1;	
	}
}

void set_EN(unsigned char en) {
	if (en >= 1) {
		GPIO_PORTD_DATA_R |= BIT_2;	
	} else {
		GPIO_PORTD_DATA_R &= ~BIT_2;	
	}
}

void set_data_bits(unsigned char data) {
	unsigned char bit0 = data & BIT_0;	
	bit0 <<= 7;
	GPIO_PORTA_DATA_R |= bit0;

	unsigned char bit1 = data & BIT_1;	
	bit1 <<= 6;
	GPIO_PORTA_DATA_R |= bit1;

	unsigned char bit2 = data & BIT_2;	
	bit2 <<= 5;
	GPIO_PORTA_DATA_R |= bit2;

	unsigned char bit3 = data & BIT_3;	
	bit3 <<= 4;
	GPIO_PORTB_DATA_R |= bit3;

	unsigned char bit4 = data & BIT_4;	
	bit4 <<= 5;
	GPIO_PORTE_DATA_R |= bit4;

	unsigned char bit5 = data & BIT_5;	
	bit5 <<= 4;
	GPIO_PORTE_DATA_R |= bit5;

	unsigned char bit6 = data & BIT_6;	
	bit6 <<= 1;
	GPIO_PORTB_DATA_R |= bit6;

	unsigned char bit7 = data & BIT_7;	
	bit7 <<= 0;
	GPIO_PORTB_DATA_R |= bit7;
}

void delay() {
	for (int i = 0; i < 5; ++i);
}

void write_LCD(unsigned char data, unsigned char rs) {
	wait_on_busy();
	set_RS(rs);	
	set_RW(0);
	delay();
	set_EN(1);
	set_data_bits(data);
	delay();
	set_EN(0);
	delay();
}

void clear_display() {
	write_instruction(0x01);
}

void return_home() {
	write_instruction(0x02);
}

void write_instruction(unsigned char data) {
	write_LCD(data, 0);
}

void write_data(unsigned char data) {
	write_LCD(data, 1);	
}

void wait_on_busy() {
		GPIO_PORTB_DIR_R &= ~BIT_0;
		while (GPIO_PORTB_DATA_R & BIT_0);
		GPIO_PORTB_DIR_R |= BIT_0;
}

void init_LCD() {
	//increment cursor and display shift off
	write_instruction(0b00000110);	
	
	//display on, cursor off, blink off
	write_instruction(0b00001100);
	
	//8-bit mode, 2 lines, 5x8 dots per character
	write_instruction(0b00111000);
}

void init_ports() {
	SYSCTL_RCGCGPIO_R |= BIT_0 | BIT_1 | BIT_3 | BIT_4;
	
    GPIO_PORTA_DEN_R |= BIT_5 | BIT_6 | BIT_7;
    GPIO_PORTB_DEN_R |= BIT_0 | BIT_1 | BIT_4;
    GPIO_PORTD_DEN_R |= BIT_0 | BIT_1 | BIT_2;
    GPIO_PORTE_DEN_R |= BIT_4 | BIT_5;

    GPIO_PORTA_DIR_R |= BIT_5 | BIT_6 | BIT_7;
    GPIO_PORTB_DIR_R |= BIT_0 | BIT_1 | BIT_4;
    GPIO_PORTD_DIR_R |= BIT_0 | BIT_1 | BIT_2;
    GPIO_PORTE_DIR_R |= BIT_4 | BIT_5;
}

int main(void)
{
	init_ports();
	init_LCD();
	write_data('A');
	return 0;
}
