#include "tm4c123gh6pm.h"
#include "bits.h"

//*(((volatile unsigned long *)0x40025000) + BIT_4) = BIT_2;

//port b base: GPIO_PORTB_DATA_BITS_R
//port b direction

#define GPIODATA (*((volatile unsigned long *) 0x40025010))

int main(void)
{
    SYSCTL_RCGCGPIO_R = BIT_5;
    GPIO_PORTF_DEN_R = BIT_2;
    GPIO_PORTF_DIR_R = BIT_2;
    GPIODATA = BIT_2;

    return 0;
}

