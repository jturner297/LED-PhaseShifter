#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "stm32l476xx.h"

/**
**************************************************************************************************
* @file main.h
* @brief Header file for program main
* @author Justin Turner
* @corresponding author: Jesse Garcia
* @version Header for main.c module
* ------------------------------------------------------------------------------------------------
* Defines the constants, enums, structures, and function prototypes used for hardware configuration,
* interrupt setup, and SysTick updates.
*
**************************************************************************************************
*/
#define startSysTickTimer_MACRO (SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk)
#define RightButtonPressed ((!(GPIOA->IDR & (0x1 << 1))))//macro
#define LeftButtonPressed ((!(GPIOA->IDR & (0x1 << 4)))) //macro
#define SpecialButtonPressed ((!(GPIOC->IDR & (0x1 << 13))))

#define NUM_of_LEDS 16

#define GPIOApins_used 5 //number of GPIOA pins used for LEDs
#define GPIOBpins_used 6 //number of GPIOB pins used for LEDs
#define GPIOCpins_used 5 //number of GPIOC pins used for LEDs

#define SYS_CLK_FREQ 4000000// default frequency of the device = 4 MHZ
#define cntclk 1000



#define DEBOUNCE_DELAY 50//arbitrary value that provided an appropriate delay while still being responsive

//enums
enum SPEED { ONE_HZ = 0, TWO_HZ, FOUR_HZ, EIGHT_HZ, SIXTEEN_HZ };
enum modes { FLASH_LED_MODE = 0, MOVE_LED_MODE = 1 };
enum choices { Left_Pushed = 1, Right_Pushed, Special_Pushed };


//Structures
struct UserInput{
	volatile uint32_t choice; //Determines which button was pressed
	volatile uint32_t debounce_counter; //Counter for debouncing
	volatile uint32_t press_pending; //Flag for pending button press
};

struct Light_Emitting_Diode{
	GPIO_TypeDef *port; //GPIOx
	uint32_t pin; //Pin number
};

//Function Prototypes
void configure_LEDS(GPIO_TypeDef *port, uint32_t pins[], uint32_t number_of_pins, uint32_t port_clock_num);
void configure_switches(void);
void configureSysTickInterrupt(void);
void updateSysTick(uint32_t ReloadValue);

#endif /* MAIN_H */
