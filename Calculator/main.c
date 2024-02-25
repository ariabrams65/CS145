#include "tm4c123gh6pm.h"
#include "bits.h"
#include <stdlib.h>
#include <stdio.h>

char matrix[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

char bit_map[9] = {0, 0, 1, 0, 2, 0, 0, 0, 3};

char get_pressed_button() {		
	char output = BIT_0;
	for (; output <= BIT_3; output <<= 1) {
		GPIO_PORTF_DATA_R = output;			
		char input = GPIO_PORTD_DATA_R;
		if (input > 0x00) {
			return matrix[bit_map[output]][bit_map[input]];				
		}
	}	
	return '\0';
}

char get_pressed_button_blocking() {
	char button = get_pressed_button();
	while (button == '\0') {
		button = get_pressed_button();
	}
	while(get_pressed_button() != '\0');	
	return button;
}

void set_RS(unsigned char rs) {
	if (rs >= 1) {
		GPIO_PORTE_DATA_R |= BIT_0;	
	} else {
		GPIO_PORTE_DATA_R &= ~BIT_0;	
	}
}

void set_RW(unsigned char rw) {
	if (rw >= 1) {
		GPIO_PORTE_DATA_R |= BIT_1;	
	} else {
		GPIO_PORTE_DATA_R &= ~BIT_1;	
	}
}

void set_EN(unsigned char en) {
	if (en >= 1) {
		GPIO_PORTE_DATA_R |= BIT_2;	
	} else {
		GPIO_PORTE_DATA_R &= ~BIT_2;	
	}
}

void set_data_bits(unsigned char data) {
	GPIO_PORTB_DATA_R = data;
}

void delay() {
	int i;
	for (i = 0; i < 1000; ++i);
}

void wait_on_busy() {
		GPIO_PORTB_DIR_R &= ~BIT_7;
		while (GPIO_PORTB_DATA_R & BIT_7);
		GPIO_PORTB_DIR_R |= BIT_7;
}

void write_LCD(unsigned char data, unsigned char rs) {
	wait_on_busy();
	set_RS(rs);	
	set_RW(0);
	delay();
	set_EN(1);
	delay();
	set_data_bits(data);
	delay();
	set_EN(0);
	delay();
}

void turn_on_LED() {
	GPIO_PORTF_DATA_R |= BIT_2;
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

void write_char(unsigned char data) {
	write_LCD(data, 1);	
}

void write_str(char* str) {
	while (*str != '\0') {
		write_char(*str);	
		++str;
	}
}

void init_LCD() {
	//increment cursor and display shift off
	write_instruction(0b00000110);	

	//display on, cursor off, blink off
	// write_instruction(0b00001100);
	
	//cursor and blink on
	write_instruction(0b00001111);
	
	//8-bit mode, 2 lines, 5x8 dots per character
	write_instruction(0b00111000);

	clear_display();
}

void init_ports() {
	SYSCTL_RCGCGPIO_R |= BIT_1 | BIT_3 | BIT_4 | BIT_5;
	
	GPIO_PORTB_DEN_R |= 0xff;
	GPIO_PORTB_DIR_R |= 0xff;

	GPIO_PORTD_DEN_R |= 0xf;
	GPIO_PORTD_DIR_R &= ~0xf;
	GPIO_PORTD_PDR_R |= 0xf;

	GPIO_PORTE_DEN_R |= 0x07;
	GPIO_PORTE_DIR_R |= 0x07;

	GPIO_PORTF_DEN_R |= 0xf;
	GPIO_PORTF_DIR_R |= 0xf;
}

void cursor_to_bottom_row() {
	write_instruction(0xC0);	
}

void run_calculator() {
	char* A = "0\0\0\0\0\0\0\0";
	char* B = "0\0\0\0\0\0\0\0";
	char state = 'A';
	
	int i = 0;
	while (1) {
		char button = get_pressed_button_blocking();
		if (button == '*' && state == 'A') {
			state = 'B';
			i = 0;
			clear_display();	
		}	
		if (button == '#') {
			break;	
		}	
		if (button == 'C') {
			A = "0\0\0\0\0\0\0\0";
			B = "0\0\0\0\0\0\0\0";
			state = 'A';
			i = 0;
			clear_display();
		}	

		if (state == 'A') {
			A[i] = button;			
		} else {
			B[i] = button;	
		}
		if (i == 8) {
			if (state == 'A') {
				state = 'B';
				i = 0;
				clear_display();	
			} else {
				break;	
			}
		}
	}
	clear_display();
	cursor_to_bottom_row();
	char product[17];
	// itoa(atoi(A) * atoi(B), product, 10);
	sprintf(product, "%d", atoi(A) * atoi(B));
	write_str(product);
}

int main(void)
{
	init_ports();
	init_LCD();
	run_calculator();

	return 0;
}
