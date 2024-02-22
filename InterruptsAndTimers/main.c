#include "tm4c123gh6pm.h"
#include "bits.h"

#define GPIODATA_PF2 (*((volatile unsigned long *) 0x40025010))

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
		GPIO_PORTB_DATA_R = output << 4;			
		char input = GPIO_PORTB_DATA_R & 0xf;
		if (input > 0x00) {
			return matrix[bit_map[output]][bit_map[input]];				
		}
	}	
	return '\0';
}

void timer0A_setup() {
	//Clocking timer 0
	SYSCTL_RCGCTIMER_R = BIT_0; 

	//Clearing whole register to disable timer
	TIMER0_CTL_R = 0x00;

	//Selecting the 16 bit timer
	TIMER0_CFG_R = BIT_2;
	
	//Setting mode to periodic
	//Setting direction to count down
	TIMER0_TAMR_R = BIT_1;
	
	//Setting prescalar
	TIMER0_TAPR_R = 245;
	
	//Setting InitCnt
	TIMER0_TAILR_R = 65306;
	
	//Clearing the output flag for timer A
	TIMER0_ICR_R = BIT_0;


	//Enables interrupts for timer 0
	TIMER0_IMR_R = BIT_0;	

	//Enables interrupt form Timer 0A
	NVIC_EN0_R |= BIT_19;

	//Enables timer
	TIMER0_CTL_R = BIT_0;
}

void timer0A_handler(void) {		
	GPIODATA_PF2 = ~(GPIODATA_PF2 & BIT_2) & BIT_2;	
    TIMER0_ICR_R = BIT_0;
}

void pushbutton_handler(void) {
	UART_Tx('$');
	GPIO_PORTE_ICR_R = BIT_0;
}

void UART_setup() {
    // 1. Enable the UART module using the RCGCUART register (see page 344).
    SYSCTL_RCGCUART_R = BIT_0;


    // 3. Set the GPIO AFSEL bits for the appropriate pins (see page 671). To determine which GPIOs to
    // configure, see Table 23-4 on page 1344.
    GPIO_PORTA_AFSEL_R = BIT_0 | BIT_1;

    // 5. Configure the PMCn fields in the GPIOPCTL register to assign the UART signals to the appropriate
    // pins (see page 688 and Table 23-5 on page 1351).
    GPIO_PORTA_PCTL_R = BIT_0 | BIT_4;
    
    GPIO_PORTA_DEN_R = BIT_0 | BIT_1;

    UART0_CTL_R &= ~BIT_0; //clears the first bit in UARTCTL
    UART0_IBRD_R = 104;
    UART0_FBRD_R = 11;
    UART0_LCRH_R = UART_LCRH_WLEN_8;
    UART0_CC_R = 0x0; 
    
    UART0_CTL_R = BIT_0 | BIT_8 | BIT_9;
}

UART_Tx(char data) {
    while (UART0_FR_R & BIT_5);
        UART0_DR_R = data;
}

int main(void)
{
	SYSCTL_RCGCGPIO_R = BIT_0 | BIT_1 | BIT_4 | BIT_5;

    GPIO_PORTF_DEN_R = BIT_2;
    GPIO_PORTF_DIR_R = BIT_2;
	GPIODATA_PF2 = 0x00;
	
	GPIO_PORTB_DEN_R = BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6 | BIT_7;
	GPIO_PORTB_DIR_R = BIT_4 | BIT_5 | BIT_6 | BIT_7;
	
	GPIO_PORTE_DEN_R = BIT_0;
	GPIO_PORTE_DIR_R = 0x00;
	
	// GPIO_PORTE_PUR_R |= BIT_0;
	
	//Set PE0 interrupts to edge-sensitive
	GPIO_PORTE_IS_R = 0x00; 
	
	//Set PE0 interrupts to sense rising edge
	GPIO_PORTE_IEV_R = BIT_0;
	
	//clears interrupt flag
	GPIO_PORTE_ICR_R = BIT_0;
	
	//Enabling intterupts for PE0
	GPIO_PORTE_IM_R = BIT_0;

	NVIC_EN0_R |= BIT_4;

	UART_setup();
	timer0A_setup();

	while(1) {
		char button = get_pressed_button();
		if (button != '\0') {
			UART_Tx(button);
			while (get_pressed_button() != '\0') {}
		}
	}
		
	return 0;
}
