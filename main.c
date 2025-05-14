#include <stdint.h>
#include <stm32l476xx.h>
#include "main.h"
/**
**************************************************************************************************
* @file main.c
* @brief Main program body
* @author: Justin Turner
* @corresponding author: Jesse Garcia
* @version 1.9
**************************************************************************************************
*/
const uint32_t FLASHfrequency[] = {1, 2, 4, 8, 16};
volatile uint32_t Blink_Phase = 0; //value that controls which half of the LEDs are lit



enum SPEED speeds = ONE_HZ; //set default speed for mode 1
enum modes system_state = FLASH_LED_MODE; //set default mode as Flashing

struct UserInput button = {0, 0, 0}; //Creates a button with choice, counter, pending values at 0;

volatile uint32_t msTimer = 0;

struct Light_Emitting_Diode LEDS[] = { //declaring the array of LEDS
		 {GPIOC, 8},  {GPIOC, 9}, {GPIOC, 6}, {GPIOB, 8},
		 {GPIOC, 5}, {GPIOB, 9},  {GPIOA, 12}, {GPIOA, 11},
		 {GPIOA, 6}, {GPIOB, 12}, {GPIOA, 7}, {GPIOB, 11},
		 {GPIOB, 6},  {GPIOC, 7}, {GPIOB, 2}, {GPIOA, 9},
};

//================================================================================================
// configure_LEDS()
//
// @parm:   *port = GPIOx
//	    pins[] = Array containing the port pin numbers used
//          number_of_pins = Number of port pins used
//	    port_clock_num = The number associated with the port clock "0 for port A"
// @return: none
// 		Conveniently configures multiple LEDs at once. Also assists with board wire management.
//================================================================================================
void configure_LEDS (GPIO_TypeDef *port, uint32_t pins[], uint32_t number_of_pins, uint32_t port_clock_num)
{
	RCC->AHB2ENR |= (0x1 << port_clock_num);
	for (uint32_t i = 0; i < number_of_pins; i++){
		port->MODER &= ~(0x3 << (2 * pins[i]));
		port->MODER |= (0x1 << (2 * pins[i]));
		port->OTYPER &= ~(0x1 << pins[i]);
		port->OSPEEDR &= ~(0x3 << (2 * pins[i]));
		port->PUPDR &= ~(0x3 << (2 * pins[i]));
	}
}
//================================================================================================
// configure_external_switches()
//
// @parm: none
// @return: none
//
// 		Setups 2 external push button switches at PA4 and PA1 for input. Also configures external
//		interrupts for pins 4 and 1
//
//  		Note: Left Button = PA4, Right Button = PA1
//================================================================================================
void configure_external_switches(void)
{
		//RCC->AHB2ENR |= (0x1 << 0); //Port A - already enabled in previous function
		GPIOA->MODER &= ~(0x3 << (2 *4 ));//input pin (PA4 = 00)
		GPIOA->PUPDR = (GPIOA->PUPDR &= ~(0x3 << (2*4)))|(0x1 << 2 * 4);//toggles pull-up (PA4=01)
		//configure switch #2 PA1 for input
		GPIOA->MODER &= ~(0x3 << (2 * 1));//input pin (PA1 = 00)
		GPIOA->PUPDR = (GPIOA->PUPDR &= ~(0x3 << (2*1)))|(0x1 << 2 * 1);//toggles pull-up (PA1=01)

	    RCC->APB2ENR |= (0x1 << 0); //Enable system configuration clock for external interrupts
		SYSCFG->EXTICR[0] |= (0x0 << 4);//setup PA1 as target pin for EXTI1
		SYSCFG->EXTICR[1] |= (0x0 << 0);//setup PA4 as target pin for EXTI4
     	EXTI -> FTSR1 |= (0x1 << 4)| (0x1 << 1); //enable falling edge trigger detection
     	EXTI -> IMR1 |= (0x1 << 4)|(0x1 << 1); //Unmask EXTI4 and EXTI1
    	NVIC_SetPriority(EXTI4_IRQn, 6); //pin 4 interrupt: priority level 6
    	NVIC_SetPriority(EXTI1_IRQn, 5);//pin 1 interrupt: priority level 5
    	NVIC_EnableIRQ(EXTI4_IRQn); //enable interrupt at pin 4
    	NVIC_EnableIRQ(EXTI1_IRQn); //enable interrupt at pin 1
}

void configure_Board_Button (void)
{
	//RCC->AHB2ENR |= (0x1 << 2); //Port C -already enabled in previous function
	GPIOC->MODER &= ~(0x3 << (2*13));
	GPIOC->PUPDR &= ~(0x3 << (2*13)); //Already uses hardware pull-up

	//RCC->APB2ENR |= (0x1 << 0); //Already done in previous function
	SYSCFG->EXTICR[3] |= (0x2 << 4); //setup PC13 as target pin for EXTI15_10
	EXTI -> FTSR1 |= (0x1 << 13);//enable falling edge trigger detection
	EXTI->IMR1 |= (0x1 << 13);//Unmask EXTI15_10
	NVIC_SetPriority(EXTI15_10_IRQn, 2);//pin 13 interrupt: priority level 2
	NVIC_EnableIRQ(EXTI15_10_IRQn); //enable interrupt at pin 13

}

//================================================================================================
// configureSysTickInterrupt()
//
// @parm: none
// @return: none
//
// 		Configures the hardware so the SysTick timer will trigger every 1ms
//================================================================================================
void configureSysTickInterrupt(void)
{
	SysTick->CTRL = 0; //disable SysTick timer
	NVIC_SetPriority(SysTick_IRQn, 7); //set priority level at 7
	SysTick->LOAD = 3999; //set the counter reload value 1ms
	SysTick->VAL = 0; //reset SysTick timer value
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; //use system clock
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //enable SysTick interrupts
}
//================================================================================================
// updateARR()
//
// @parm: ReloadValue = Value used in the ARR (determines delay)
// @return: none
//
// 		Updates the ARR value and resets the counter
//================================================================================================
void updateARR (uint32_t ReloadValue){
	  TIM2->ARR = ((cntclk/ReloadValue) - 1); //update SysTick load register
	  TIM2->CNT = 0;
}

//================================================================================================
// configureTIM2()
//
// @parm: none
// @return: none
//
// 		Configures the general purpose timer TIM2
//================================================================================================
void configureTIM2 (void)
{
	  RCC->APB1ENR1 |= (1 << 0);  // Enable TIM2 clock
	  TIM2->PSC = (SYS_CLK_FREQ/cntclk -1);
	  updateARR(FLASHfrequency[ONE_HZ]);
	  TIM2->DIER |= (1 << 0);          // Enable update interrupt
	     TIM2->CR1 |= (1 << 0);           // Start timer
	     NVIC_SetPriority(TIM2_IRQn, 3);
	     NVIC_EnableIRQ(TIM2_IRQn);       // Enable interrupt in NVIC
}


int main(void)
{
	//--initialize hardware------------------------------------------------------------------------
	uint32_t GPIOA_pins[] = {9,7,6,12,11}; //LEDs connected to port A's pins
	uint32_t GPIOB_pins[] = {8,9,12,11,6,2}; //LEDs connected to port B's pins
	uint32_t GPIOC_pins[] = {8,9,6,5,7};//LEDs connected to port C's pins

	configure_LEDS(GPIOA, GPIOA_pins, GPIOApins_used, 0);//configure port A's LEDs
	configure_LEDS(GPIOB, GPIOB_pins, GPIOBpins_used, 1);//configure port B's LEDs
	configure_LEDS(GPIOC, GPIOC_pins, GPIOCpins_used, 2);//configure port C's LEDs
	configure_external_switches();//configure the switches and their dedicated interrupts
	configureSysTickInterrupt();
	configure_Board_Button(); //configure the board button and its interrupt
	configureTIM2(); //configure general purpose TIM2
	startSysTickTimer_MACRO;

	//---------------------------------------------------------------------------------------------
	// Whenever the external interrupts go off, the button choice is recorded and the pending flag
	// and debounce counter is set. This polling loop continually checks for that information and
	// executes the appropriate actions based on that. The first action is to decrement the debounce
	// counter until it reaches 0. Once 0, the debouncing is finished and the button choice can be
	// processed. For extra refinement, the program double checks the button state by polling.
	//---------------------------------------------------------------------------------------------

	uint32_t LEDcount = 0;//Sets the first LED that will be turned on

	while (1)
	{
		if(button.press_pending == 1 ){ //if a button press is pending....
			if(msTimer - button.debounce_counter >= DEBOUNCE_DELAY ){//if the debounce counter is 0....
				switch(button.choice){

				case Left_Pushed: //left button pressed
						if (system_state == FLASH_LED_MODE && speeds > 0){//if the system in FLASH_LED_MODE....
							updateARR(FLASHfrequency[speeds-=1]);//Half Speed
						}
						else if (system_state == MOVE_LED_MODE){//if the system in MOVE_LED_MODE....
							LEDcount = (LEDcount + 1)%16; //increment the LEDcount
							if (LEDcount > 0){ //if the LEDcount is greater than 0
								LEDS[LEDcount - 1].port-> ODR &= ~(0x1 << LEDS[LEDcount - 1].pin);//turn off previous LED
							}
							else{ //the LEDcount is 0....
								LEDS[15].port-> ODR &= ~(0x1 << LEDS[15].pin); //turn off the sixteenth LED
							}
						}					
					break;

				case Right_Pushed: //right button pressed
						if (system_state == FLASH_LED_MODE && speeds < 4){//if the system in FLASH_LED_MODE....
							updateARR(FLASHfrequency[speeds+=1]);//Double Speed
						}
						else if (system_state == MOVE_LED_MODE){//if the system in MOVE_LED_MODE....
							LEDcount = (LEDcount + 15)%16; //decrments the LEDcount
							if (LEDcount < 15){ //if LEDcount is less than 15....
								LEDS[LEDcount + 1].port-> ODR &= ~(0x1 << LEDS[LEDcount + 1].pin); //turn off previous LED
							}
							else{ //the LEDcount is 15....
								LEDS[0].port-> ODR &= ~(0x1 << LEDS[0].pin); //turn off the first LED
							}

						}
					
					break;

				case Special_Pushed: //Special button pressed
					for(uint32_t z = 1; z < NUM_of_LEDS; z++){
						LEDS[z].port-> ODR &= ~(0x1 << LEDS[z].pin);//turn off every LED except the first
					}
					LEDcount = 0; //reset LEDcount so that the MOVE_LED_MODE begins with the rightmost LED
					updateARR(FLASHfrequency[speeds=0]);

					Blink_Phase = 0; //reset the Blink Phase so that the FLASH_LED_MODE begins with the right half
					system_state^=1; //toggle system state

					if (system_state == FLASH_LED_MODE) { //if the system_state has changed to FLASJ_LED_MODE....
					    TIM2->CNT = 0; //restart the count
					    TIM2->EGR |= (1 << 0);  // force update event /manual trigger
					}
					break;
				}//end switch statement
				button.press_pending = 0; // clear the pending flag
				button.debounce_counter = 0;
			}
		}
		if (system_state == MOVE_LED_MODE){ //if the system state is in MOVE_LED_MODE...
			LEDS[LEDcount].port-> ODR |= (0x1 << LEDS[LEDcount].pin); //turn on current LED
		}
	}
}//end main







//================================================================================================
// EXTI4_IRQHandler()
//
// @parm: none
// @return: none
//
// 	 	Button choice is flagged as "Left_Pushed", a single left button
//		press. Now with the button choice locked in, the button can begin debouncing.
//
//================================================================================================
void EXTI4_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 4)) {//if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 4);  // Clear interrupt flag
		  button.choice = Left_Pushed; //left button chosen
		  button.press_pending = 1; //flag that determines if a press is pending
		  button.debounce_counter = msTimer;//sets the button's debounce counter to the delay value
	}
}

//================================================================================================
// EXTI1_IRQHandler()
//
// @parm: none
// @return: none
//
// 	 	Button choice is flagged as "Right_Pushed", a single right button
//		press. Now with the button choice locked in, the button can begin debouncing.
//
//================================================================================================
void EXTI1_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 1)) { //if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 1);  // Clear interrupt flag
		  button.choice = Right_Pushed; //right button chosen
		  button.press_pending = 1; //flag that determines if a press is pending
		  button.debounce_counter = msTimer;//sets the button's debounce counter to the delay value
	}
}

//================================================================================================
// EXTI1_IRQHandler()
//
// @parm: none
// @return: none
//
// 		Button choice is flagged as "Special_Pushed", a single specia button
//		press. Now with the button choice locked in, the button can begin debouncing.
//
//================================================================================================
void EXTI15_10_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 13)) { //if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 13);  // Clear interrupt flag
		  button.choice = Special_Pushed; //both buttons chosen
		  button.press_pending = 1; //flag that determines if a press is pending
		  button.debounce_counter = msTimer;//sets the button's debounce counter to the delay value
	}
}
//================================================================================================
// SysTick_Handler()
//
// @parm: none
// @return: none
//
// 		Increments msTimer variable
//================================================================================================
void SysTick_Handler(void)
{
	msTimer++; //goes up every 1ms
}

//================================================================================================
// TIM2_IRQHandler()
//
// @parm: none
// @return: none
//
// 		When the left 8 LEDs are lit, the other half are off. Every time the TIM2 interrupt 
//		triggers one side's LEDs will light up while the other's will turn off.
//================================================================================================
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & (1 << 0)) {
        TIM2->SR &= ~(1 << 0);            // Clear update flag
        if (system_state == FLASH_LED_MODE ){ //if system_state is 0
        		static const uint32_t high_end_values[] = {8,16}; //assign higher limits
        		static const uint32_t low_end_values[] = {0,8}; //assign lower limits
        		uint32_t i = low_end_values[Blink_Phase]; //variable used for turning on one half of the LEDs
        		uint32_t j = low_end_values[Blink_Phase^1]; //variable used for turning off the other half of the LEDs

        		while(i < high_end_values[Blink_Phase] && j < high_end_values[Blink_Phase^1]){
        			LEDS[j].port-> ODR &= ~(0x1 << LEDS[j].pin); //turns off the led on one half;
        			j++;
        			LEDS[i].port-> ODR |= (0x1 << LEDS[i].pin); //turns on the led on the other half;
        			i++;
        		}
        		Blink_Phase^=1;//Toggle Blink Phase
        	}
    }
}













































