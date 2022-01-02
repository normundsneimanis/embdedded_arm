// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "..//tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void) {
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x02;      // 1) B
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  GPIO_PORTB_LOCK_R = 0x4C4F434B;   // 2) unlock PortA PB0  
  GPIO_PORTB_CR_R = 0x3F;           // allow changes to PB4-0  
  GPIO_PORTB_AMSEL_R = 0x00; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R = 0x00000000; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R = 0x3F;   // 5) inputs on PB5-0
	GPIO_PORTB_DR8R_R |= 0xFF;	// 8 mA out on PB5-0
  GPIO_PORTB_AFSEL_R = 0x00; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data) {
  GPIO_PORTB_DATA_R = data;
}
