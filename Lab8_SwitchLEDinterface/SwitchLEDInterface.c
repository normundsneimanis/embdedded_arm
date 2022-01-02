// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

int buttonStatus;

// ***** 3. Subroutines Section *****
void Delay100ms(unsigned long time){
  unsigned long i;
  while(time > 0){
    i = 1333333;  // this number means 100ms
    while(i > 0){
      i = i - 1;
    }
    time = time - 1; // decrements every 100 ms
  }
}

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).

int main(void){ unsigned long volatile delay;
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
  // initialization goes here
	// Turn on the clock for Port F
	SYSCTL_RCGC2_R |= 0x00000010;
	delay = SYSCTL_RCGC2_R;
	//	Clear the PF4 and PF2 bits in Port F AMSEL to disable analog
	GPIO_PORTE_AMSEL_R = 0x00;
	//	Clear the PF4 and PF2 bit fields in Port F PCTL to configure as GPIO
	GPIO_PORTE_PCTL_R = 0x00000000;
	//	Set the Port F direction register so
	//		PE1 is an output and
	//		PE0 is an input
	GPIO_PORTE_DIR_R = 0x02;
	//	Clear the PF4 and PF2 bits in Port F AFSEL to disable alternate functions
	GPIO_PORTE_AFSEL_R = 0x00;
	//	Set the PE0 and PE1 bits in Port F DEN to enable digital
	GPIO_PORTE_DEN_R = 0x03;
	//	Set the PF4 bit in Port F PUR to activate an internal pullup resistor
	//GPIO_PORTE_PUR_R = 0x10;
	//	Set the PF2 bit in Port F DATA so the LED is initially ON
	GPIO_PORTE_DATA_R = 0x02;
	

  EnableInterrupts();           // enable interrupts for the grader
  while(1){
		Delay100ms(1);
		// Read the switch and test if the switch is pressed
		buttonStatus = GPIO_PORTE_DATA_R & 0x01;
		// If PF4=0 (the switch is pressed),
		// 		toggle PF2 (flip bit from 0 to 1, or from 1 to 0)
		// If PF4=1 (the switch is not pressed),
		//		set PF2, so LED is ON
		if (buttonStatus) {
			if (GPIO_PORTE_DATA_R & 0x02) {
				GPIO_PORTE_DATA_R &= ~0x02;
			} else {
				GPIO_PORTE_DATA_R |= 0x02;
			}
		} else {
			GPIO_PORTE_DATA_R |= 0x02;
		}
  }
}
