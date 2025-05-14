### LED PhaseShifter
**LED PhaseShifter** is a project for the **STM32L476RG** that uses interrupts, SysTick, and GPIO to control 16 external LEDs. It features flashing/shifting LED pattern controlled by two pushbuttons. 



🔄 **Dual operating modes**
  - **FLASH_LED_MODE** – LEDs flash in alternating left/right pattern with an adjustable frequnecy from 1 Hz to 16 Hz
  - **MOVE_LED_MODE** – Manual LED control for shifting the LED left and right based on button input

⚡ **User-Driven Controls**
- In **FLASH_LED_MODE**:
  - Left button: Halves Frequency
  - Right button: Doubles Frequency
- In **MOVE_LED_MODE**:
  - Left button: shifts LED position left
  - Right button: shifts LED position right

🔁 **Instant response mode switching**
 - Pressing the built-in USER button instantly toggles between FLASH_LED_MODE and MOVE_LED_MODE

🕒 **Real-time processing with timers**
  - TIM2 used to control LED animation
  - SysTick used for millisecond timekeeping and debouncing
    
👆 **Interrupt-based input handling**
  - External interrupts on PA1, PA4, and PC13
  - Software debouncing ensures clean button logic
    
🧪 **Bare-Metal Embedded C**
  - Written without HAL; uses direct register manipulation (CMSIS)


## Hardware Requirements
- STM32 NUCLEO-L476RG development board
- 16 external LEDs:
  - 8 LEDs for SIDE A
  - 8 LEDs for SIDE B
- 2 pushbuttons (connected to PA4 and PA1)
- Breadboard and jumper wires
- USB cable for power and flashing via ST-Link****
