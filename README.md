# LED PhaseShifter
<img src="assets/LED-PhaseShifter thumbnail.jpg" alt="Game Thumbnail" width="500"/> 
**LED PhaseShifter** is real-time LED control project for the **STM32L476RG** using interrupts, SysTick, and GPIO to control 16 external LEDs. Features dual operating modes: FLASH mode for alternating LED animation and MOVE mode for manual LED shifting.

## Demos
<table>
  <tr>
    <td>
  <a href="https://www.youtube.com/watch?v=t95wCDicaAc">
    <img src="assets/flash mode image a.jpg" width="300%" />
      </a>
    </td>
    <td>
    <a href="https://www.youtube.com/watch?v=3YkH4LqlbU4">
    <img src="assets/move mode image a.jpg" width="300%" />
      </a>
    </td>
  </tr>
  <tr>
    <td><sub><strong>Flash LED Mode</strong></sub></td>
    <td><sub><strong>Mode Switching & Move LED Mode</strong></sub></td>
  </tr>
</table>


## Features
🔄 **Dual operating modes**
  - **FLASH LED MODE** – LEDs flash in alternating left/right pattern with an adjustable frequnecy from 1 Hz to 16 Hz
  - **MOVE LED MODE** – Manual LED control for shifting the LED left and right based on button input

⚡ **User-Driven Controls**
- In **FLASH LED MODE**:
  - Left button: Halves Frequency
  - Right button: Doubles Frequency
- In **MOVE LED MODE**:
  - Left button: shifts LED position left
  - Right button: shifts LED position right

🔁 **Instant response mode switching**
 - Pressing the built-in USER button instantly toggles between FLASH LED MODE and MOVE LED MODE

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
- 16 Current-limiting resistors for each LED (220-470Ω)
- 2 external pushbuttons
  - Connected to PA4 (left) and PA1 (right)
  - Internally pulled-up
- Built-in USER button (PC13)
- Breadboard and jumper wires
- USB cable for power and flashing via ST-Link****
