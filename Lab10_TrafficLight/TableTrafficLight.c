// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
	
#define SENSOR  (*((volatile unsigned long *)0x4002400C))
#define LIGHT   (*((volatile unsigned long *)0x400050FC))
// Linked data structure
struct State {
  unsigned long carsLight;  // 6-bit pattern to output
	unsigned long pedestriansLight; // 2-bit pattern to pedestrians light output
  unsigned long Time; // delay in 10ms units 
  unsigned long Next[8]; // next state for inputs 0,1,2,3,4,5,6,7
};

#define PEDESTRIANS_RED 0x02
#define PEDESTRIANS_GREEN 0x08
#define PEDESTRIANS_OFF 0

/*
 * 0 - nothing happening
 * 1 - pedestrians present
 * 2 - east present
 * 3 - east present and pedestrians present
 * 4 - south present
 * 5 - south present and pedestrians present
 * 6 - east and south present
 * 7 - east, south, and preds present
 */

typedef const struct State STyp;
enum states {
	SouthGo0,
	SouthGo,
	SouthWait0,
	SouthWait1,
	SouthWait2,
	SouthWait3,
	SouthWait,
	EastGo0,
	EastGo,
	EastWait0,
	EastWait1,
	EastWait2,
	EastWait3,
	EastWait,
	PedestriansGo,
	PedestriansFlash0,
	PedestriansFlash1,
	PedestriansFlash2,
	PedestriansFlash3,
	PedestriansFlash4,
	PedestriansFlash5,
	PedestriansFlash6,
	numStates
};

#define WAIT_LIGHT 300
#define WAIT_FLASH 50
#define WAIT_YELLOW 100

/*
 * Walk = 4
 * South = 2
 * East = 1
 */

STyp FSM[numStates]={
	// SouthGo0 100110
	{0x26, PEDESTRIANS_RED, WAIT_YELLOW, {SouthGo, SouthGo, SouthGo, SouthGo, SouthGo, SouthGo, SouthGo, SouthGo}},
	// SouthGo South East 100001
	{0x21, PEDESTRIANS_RED, WAIT_LIGHT, {SouthGo, SouthWait0, SouthGo, SouthWait0, SouthWait0, SouthWait0, SouthWait0, SouthWait0}},
	// SouthWait0 100000
	{0x20, PEDESTRIANS_RED, WAIT_FLASH, {SouthWait1, SouthWait1, SouthWait1, SouthWait1, SouthWait1, SouthWait1, SouthWait1, SouthWait1}},
	// SouthWait1 100001
	{0x21, PEDESTRIANS_RED, WAIT_FLASH, {SouthWait2, SouthWait2, SouthWait2, SouthWait2, SouthWait2, SouthWait2, SouthWait2, SouthWait2}},
	// SouthWait2 100000
	{0x20, PEDESTRIANS_RED, WAIT_FLASH, {SouthWait3, SouthWait3, SouthWait3, SouthWait3, SouthWait3, SouthWait3, SouthWait3, SouthWait3}},
	// SouthWait3 100001
	{0x21, PEDESTRIANS_RED, WAIT_FLASH, {SouthWait, SouthWait, SouthWait, SouthWait, SouthWait, SouthWait, SouthWait, SouthWait}},
	// SouthWait 100010
	{0x22, PEDESTRIANS_RED, WAIT_YELLOW, {EastGo0, EastGo0, EastGo0, EastGo0, PedestriansGo, PedestriansGo, PedestriansGo, EastGo0}},
	// EastGo0 110100
	{0x34, PEDESTRIANS_RED, WAIT_YELLOW, {EastGo, EastGo, EastGo, EastGo, EastGo, EastGo, EastGo, EastGo}},
	// EastGo 001100
	{0x0C, PEDESTRIANS_RED, WAIT_LIGHT, {EastGo, EastGo, EastWait0, EastWait0, EastWait0, EastWait0, EastWait0, EastWait0}},
	// EastWait0 000100
	{0x04, PEDESTRIANS_RED, WAIT_FLASH, {EastWait1, EastWait1, EastWait1, EastWait1, EastWait1, EastWait1, EastWait1, EastWait1}},
	// EastWait1 001100
	{0x0C, PEDESTRIANS_RED, WAIT_FLASH, {EastWait2, EastWait2, EastWait2, EastWait2, EastWait2, EastWait2, EastWait2, EastWait2}},
	// EastWait2 000100
	{0x04, PEDESTRIANS_RED, WAIT_FLASH, {EastWait3, EastWait3, EastWait3, EastWait3, EastWait3, EastWait3, EastWait3, EastWait3}},
	// EastWait3 001100
	{0x0C, PEDESTRIANS_RED, WAIT_FLASH, {EastWait, EastWait, EastWait, EastWait, EastWait, EastWait, EastWait, EastWait}},
	// EastWait 010100
	{0x14, PEDESTRIANS_RED, WAIT_YELLOW, {SouthGo0, EastGo0, SouthGo0, SouthGo0, PedestriansGo, PedestriansGo, PedestriansGo, PedestriansGo}},
	// PedestriansGo 100100
	{0x24, PEDESTRIANS_GREEN, WAIT_LIGHT, {PedestriansGo, PedestriansFlash0, PedestriansFlash0, PedestriansFlash0, PedestriansGo, PedestriansFlash0, PedestriansFlash0, PedestriansFlash0}},
	// PedestriansFlash0 100100
	{0x24, PEDESTRIANS_OFF, WAIT_FLASH, {PedestriansFlash1, PedestriansFlash1, PedestriansFlash1, PedestriansFlash1, PedestriansFlash1, PedestriansFlash1, PedestriansFlash1, PedestriansFlash1}},
	// PedestriansFlash1
	{0x24, PEDESTRIANS_GREEN, WAIT_FLASH, {PedestriansFlash2, PedestriansFlash2, PedestriansFlash2, PedestriansFlash2, PedestriansFlash2, PedestriansFlash2, PedestriansFlash2, PedestriansFlash2}},
	// PedestriansFlash2
	{0x24, PEDESTRIANS_OFF, WAIT_FLASH, {PedestriansFlash3, PedestriansFlash3, PedestriansFlash3, PedestriansFlash3, PedestriansFlash3, PedestriansFlash3, PedestriansFlash3, PedestriansFlash3}},
	// PedestriansFlash3
	{0x24, PEDESTRIANS_GREEN, WAIT_FLASH, {PedestriansFlash4, PedestriansFlash4, PedestriansFlash4, PedestriansFlash4, PedestriansFlash4, PedestriansFlash4, PedestriansFlash4, PedestriansFlash4}},
	// PedestriansFlash4
	{0x24, PEDESTRIANS_OFF, WAIT_FLASH, {PedestriansFlash5, PedestriansFlash5, PedestriansFlash5, PedestriansFlash5, PedestriansFlash5, PedestriansFlash5, PedestriansFlash5, PedestriansFlash5}},
	// PedestriansFlash5
	{0x24, PEDESTRIANS_GREEN, WAIT_FLASH, {EastGo0, EastGo0, SouthGo0, EastGo0, EastGo0, EastGo0, SouthGo0}},
};

unsigned long S;  // index to the current state 
unsigned long Input, inputPrev; 

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void SysTick_Init(void);
void SysTick_Wait10ms(unsigned long delay);
void PortF_Init(void);

// ***** 3. Subroutines Section *****
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay) {
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0) { // wait for count flag
  }
}
// 800000*12.5ns equals 10ms
void SysTick_Wait10ms(unsigned long delay) {
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}



int main(void) {
	volatile unsigned long delay;
	int firstState = 1;
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
	SysTick_Init();   // Program 10.2
	PortF_Init();
  SYSCTL_RCGC2_R |= 0x12;      // 1) B E
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  GPIO_PORTE_AMSEL_R &= ~0x0F; // 3) disable analog function on PE1-0
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x0F;   // 5) inputs on PE1-0
  GPIO_PORTE_AFSEL_R &= ~0x0F; // 6) regular function on PE1-0
  GPIO_PORTE_DEN_R |= 0x0F;    // 7) enable digital on PE1-0
  GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
	
  EnableInterrupts();
	S = SouthGo;
	while(1) {
    LIGHT = FSM[S].carsLight;  // set lights
		GPIO_PORTF_DATA_R = FSM[S].pedestriansLight;
		delay = FSM[S].Time;
		Input = 0;
		while(delay-- > 0) {
			SysTick_Wait(800000);  // wait 10ms
			if (!Input && GPIO_PORTE_DATA_R & 0x07) {
				Input = GPIO_PORTE_DATA_R & 0x07;     // read sensors
				if (!firstState) {
					break;
				}
			}
		}
		if (!Input) {
			Input = inputPrev;
			firstState = 0;
		} else {
			firstState = 1;
		}
		inputPrev = Input;
    //SysTick_Wait10ms(FSM[S].Time);
    S = FSM[S].Next[Input];
  }
}


void PortF_Init(void) {
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}
