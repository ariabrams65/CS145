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
    // D0 = PA7
    if((data & BIT_0) == BIT_0) {
		GPIO_PORTA_DATA_R |= BIT_7;
    } else {
		GPIO_PORTA_DATA_R &= ~BIT_7;
    }

    // D1 = PA6
    if((data & BIT_1) == BIT_1) {
		GPIO_PORTA_DATA_R |= BIT_6;
    } else {
		GPIO_PORTA_DATA_R &= ~BIT_6;
    }

    // D2 = PA5
    if((data & BIT_2) == BIT_2) {
		GPIO_PORTA_DATA_R |= BIT_5;
    } else {
		GPIO_PORTA_DATA_R &= ~BIT_5;
    }

    // D3 = PB4
    if((data & BIT_3) == BIT_3) {
		GPIO_PORTB_DATA_R |= BIT_4;
    } else {
		GPIO_PORTB_DATA_R &= ~BIT_4;
    }

    // D4 = PE5
    if((data & BIT_4) == BIT_4) {
		GPIO_PORTE_DATA_R |= BIT_5;
    } else {
		GPIO_PORTE_DATA_R &= ~BIT_5;
    }

    // D5 = PE4
    if((data & BIT_5) == BIT_5) {
		GPIO_PORTE_DATA_R |= BIT_4;
    } else {
		GPIO_PORTE_DATA_R &= ~BIT_4;
    }

    // D6 = PB1
    if((data & BIT_6) == BIT_6) {
		GPIO_PORTB_DATA_R |= BIT_1;
    } else {
		GPIO_PORTB_DATA_R &= ~BIT_1;
    }

    // D7 = PB0
    if((data & BIT_7) == BIT_7) {
		GPIO_PORTB_DATA_R |= BIT_0;
    } else {
		GPIO_PORTB_DATA_R &= ~BIT_0;
    }
}

void delay() {
	int i;
	for (i = 0; i < 1000; ++i);
}

void wait_on_busy() {
		GPIO_PORTB_DIR_R &= ~BIT_0;
		while (GPIO_PORTB_DATA_R & BIT_0);
		GPIO_PORTB_DIR_R |= BIT_0;
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

void write_data(unsigned char data) {
	write_LCD(data, 1);	
}


void init_LCD() {
	//increment cursor and display shift off
	write_instruction(0b00000110);	

	//display on, cursor off, blink off
	write_instruction(0b00001100);
	
	//cursor and blink on
	// write_instruction(0b00001111);
	
	//8-bit mode, 2 lines, 5x8 dots per character
	write_instruction(0b00111000);

	clear_display();
}

void init_ports() {
	SYSCTL_RCGCGPIO_R |= BIT_0 | BIT_1 | BIT_3 | BIT_4 | BIT_5;
	
    GPIO_PORTA_DEN_R |= BIT_5 | BIT_6 | BIT_7;
    GPIO_PORTB_DEN_R |= BIT_0 | BIT_1 | BIT_4;
    GPIO_PORTD_DEN_R |= BIT_0 | BIT_1 | BIT_2;
    GPIO_PORTE_DEN_R |= BIT_4 | BIT_5;

    GPIO_PORTA_DIR_R |= BIT_5 | BIT_6 | BIT_7;
    GPIO_PORTB_DIR_R |= BIT_0 | BIT_1 | BIT_4;
    GPIO_PORTD_DIR_R |= BIT_0 | BIT_1 | BIT_2;
    GPIO_PORTE_DIR_R |= BIT_4 | BIT_5;
	
	GPIO_PORTF_DEN_R |= BIT_2;
	GPIO_PORTF_DIR_R |= BIT_2;
}

int main(void)
{
	init_ports();
	init_LCD();
	write_data('A');
	
	while (1);

	return 0;
}
