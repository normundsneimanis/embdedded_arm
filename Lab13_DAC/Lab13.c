// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);
void SysTick_Handler(void);

int main(void) { // Real Lab13 
	unsigned long input;
	float hertz, hertzPrev;
	hertz = 0;
	hertzPrev = 0;
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
// PortE used for piano keys, PortB used for DAC        
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
  EnableInterrupts();  // enable after all initialization are done
	Sound_Off();
	Sound_Tone(0);
//	while(1) {
//		SysTick_Handler();
//		delay(1000);
//	}
	
  while(1) {
// input from keys to select tone
		input = Piano_In();
		if (input & 0x01 && hertz != 523) {
			// Piano key 0: C generates a sinusoidal DACOUT at 523.251 Hz
			//hertz = 15289 / 2;
			hertz = 523.251;
		} else if (input & 0x02 && hertz != 587) {
			// Piano key 1: D generates a sinusoidal DACOUT at 587.330 Hz
			//hertz = 13621 / 2;
			hertz = 587.330;
		} else if (input & 0x04 && hertz != 659) {
			// Piano key 2: E generates a sinusoidal DACOUT at 659.255 Hz
			//hertz = 12135 / 2;
			hertz = 659.255;
		} else if (input & 0x08 && hertz != 784) {
			// Piano key 3: G generates a sinusoidal DACOUT at 783.991 Hz
			//hertz = 10204 / 2;
			hertz = 783.991;
		} else  if (!input) {
			Sound_Off();
			Sound_Tone(0);
			hertz = 0;
			hertzPrev = 0;
		}
		if (hertz != hertzPrev) {
			// 1/hertz*100000000/16/12.5
			Sound_Tone((int) (((1 / hertz) * 100000000) / 12.5 / 1.6));
			//Sound_Tone(hertz);
			hertzPrev = hertz;
		}
		delay(10);
  }
}

// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec) {
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


