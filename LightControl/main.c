#include "tm4c123gh6pm.h"
#include "bits.h"
#include <stdbool.h>

//*(((volatile unsigned long *)0x40025000) + BIT_4) = BIT_2;

#define GPIODATA_PF2 (*((volatile unsigned long *) 0x40025010))
#define GPIODATA_PF3 (*((volatile unsigned long *) 0x40025020))

int main(void)
 {
    // 1. Enable the UART module using the RCGCUART register (see page 344).
    SYSCTL_RCGCUART_R = BIT_0;

    // 2. Enable the clock to the appropriate GPIO module via the RCGCGPIO register (see page 340).
    // To find out which GPIO port to enable, refer to Table 23-5 on page 1351.
    SYSCTL_RCGCGPIO_R = BIT_0;

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


    
    SYSCTL_RCGCGPIO_R = BIT_5;
    GPIO_PORTF_DEN_R = BIT_2 | BIT_3;
    GPIO_PORTF_DIR_R = BIT_3;
    
    char mode = 'p';
    while (1) {
        bool PF2_on = GPIODATA_PF2 & BIT_2;
        if ((PF2_on && mode == 'p') || (!PF2_on && mode == 'n')) {
            GPIODATA_PF3 = BIT_3;
        } else {
            GPIODATA_PF3 = 0x00;
        }
        mode = UART_Rx(mode);
    }
}

void UART_Tx(char data) {
    while (UART0_FR_R & BIT_5);
        UART0_DR_R = data;
}

char UART_Rx(char mode) {
    char data;
    if (UART0_FR_R & BIT_4) {
        return mode; 
    }
    data = UART0_DR_R;
    return (unsigned char)data;
}
