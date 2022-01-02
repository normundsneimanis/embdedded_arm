// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"

unsigned long ticks = 0;
unsigned long toneFreq = 0;
const unsigned char SineWave[16] = {8,12,14,15,15,15,14,12,10,
                                    8,4,2,1,2,4,6};
unsigned char Index=0;           // Index varies from 0 to 15

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void) {
	DAC_Init();
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000;
	// priority 1
  NVIC_ST_CTRL_R = 0x0007; // enable,core clock, and interrupts
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period) {
// this routine sets the RELOAD and starts SysTick
	if (period) {
		NVIC_ST_RELOAD_R = period - 1;
		toneFreq = period - 1;
	}
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void) {
 // this routine stops the sound output
	NVIC_ST_RELOAD_R = 0;
	GPIO_PORTA_DATA_R = 0x00;
	ticks = 0x00;
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void) {
  Index = (Index+1)&0x0F;      // 4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3... 
  DAC_Out(SineWave[Index]);    // output one value each interrupt
	ticks = SineWave[Index];
}
