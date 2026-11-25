<h1 align="center">Two-Road Traffic Light System on XNUCLEO-F103RB</h1>

This repository contains the course project of Microprocessors – Microcontrollers (CO3009): a traffic light controller for a two-road intersection running on the XNUCLEO-F103RB board (STM32F103RB). The system supports automatic, manual and timing adjustment modes, reads the DHT20 sensor to show temperature and humidity, and displays real-time information on a 16x2 I2C LCD. All work is executed as tasks on an O(1) cooperative scheduler driven by a 10 ms timer interrupt, and every mode is managed by a Finite State Machine (FSM). Push buttons are read with software debouncing.

Table of Contents
Project Overview
Features
Hardware Requirements
Languages and Tools
Project Structure
Mode Descriptions
How to Build and Run
Implementing on Hardware

---
1. Project Overview
The system controls the traffic lights of a two-road intersection with the following components:
2 traffic light modules (one per road). Each module is driven by 2 signal pins, giving 4 states: Red, Amber, Green and Off.
1 LCD 16x2 (I2C) to show the remaining time of each road, the current mode and the sensor values.
1 DHT20 sensor (I2C) to measure temperature and humidity.
4 push buttons (2 button modules):
Mode: cycles through the modes
Next / Increase: changes the light state in manual mode, or increases the duration in tuning modes
Set: confirms and stores the chosen duration
Spare: configured and tested, reserved for future use
The whole program runs on an O(1) scheduler (`SCH_Add_Task`, `SCH_Delete_Task`, `SCH_Update`, `SCH_Dispatch_Tasks`) with support for one-shot tasks. Hardware access is kept inside the driver modules, while the FSMs only call the module functions.
Main Goals
Automatic Traffic Light Operation with configurable Red, Amber and Green durations.
Manual Control of the light state with push buttons.
On-the-Fly Modification of the Red, Amber and Green durations, shown on the LCD.
Environment Display of temperature and humidity from the DHT20 sensor.
Safe Operation: the two roads are never Green at the same time.
---
2. Features
Operating Modes:
Normal Mode: automatic traffic light sequence
Manual Mode: step through the light states with a button
Modify Red Duration
Modify Amber Duration
Modify Green Duration
Real-Time Display on LCD 16x2 using the I2C protocol, showing:
Remaining time of the light on each road.
Temperature and humidity in Normal Mode.
Current mode and the value being edited in the other modes.
O(1) Scheduler: tasks are kept in an array sorted by run time, each storing a delay relative to the previous task, so `SCH_Update` only checks the first task. One-shot tasks are removed automatically after running.
Non-blocking DHT20 Reading: the measurement is triggered every 2 s and the result is read by a one-shot task 80 ms later, without using `HAL_Delay`. Each reading is checked with CRC-8.
Blinking Lights in modification modes (2 Hz) to indicate the editing state and which color is being edited.
Button Debouncing using three consecutive readings (10 ms apart) for all push buttons.
Timing Constraint: when returning to Normal Mode, the Red duration is recalculated as Red = Green + Amber, so the two roads always stay synchronized.
---
3. Hardware Requirements
XNUCLEO-F103RB board (STM32F103RB) with the GROVE expansion board
2 traffic light modules (2 signals each)
2 button modules (2 push buttons each)
1 LCD 16x2 with I2C backpack (address `0x21`)
1 DHT20 temperature and humidity sensor (address `0x38`)
Pin Mapping
Arduino pin	STM32 pin	GPIO mode	User Label	Function
D3	PB3	Output Push-Pull	`D3`	Road 1 – signal A
D4	PB5	Output Push-Pull	`D4`	Road 1 – signal B
D5	PB4	Output Push-Pull	`D5`	Road 2 – signal A
D6	PB10	Output Push-Pull	`D6`	Road 2 – signal B
D7	PA8	Input, Pull-up	`A0`	Button 1 – Mode
D8	PA9	Input, Pull-up	`A1`	Button 2 – Next / Increase
D9	PC7	Input, Pull-up	`A2`	Button 3 – Set
D10	PB6	Input, Pull-up	`A3`	Button 4 – Spare
D15 / D14	PB8 / PB9	I2C1 SCL / SDA	—	LCD and DHT20
—	PA5	Output	`LED`	Scheduler heartbeat LED
Signal combination of a traffic light module (A, B): `00` Off · `01` Amber · `10` Green · `11` Red.
---
4. Languages and Tools
Languages
C (application code, STM32 HAL)
Assembly (startup file)
Tools
STM32CubeIDE: STM32CubeIDE Download
STM32CubeProgrammer: STM32CubeProgrammer Download
Git / Sourcetree for version control
Ensure your development environment is configured for STM32 microcontrollers.
---
5. Project Structure
```
.
├── Implementation/                          # STM32CubeIDE project
│   ├── .settings/                           # Project settings and preferences
│   ├── Core/                                # Core source code and headers
│   │   ├── Inc/                             # Header files
│   │   │   ├── button.h
│   │   │   ├── dht20.h
│   │   │   ├── fsm_automatic.h
│   │   │   ├── fsm_manual.h
│   │   │   ├── fsm_tuning.h
│   │   │   ├── global.h
│   │   │   ├── lcd.h
│   │   │   ├── led_states.h
│   │   │   ├── main.h
│   │   │   ├── scheduler.h
│   │   │   ├── software_timer.h
│   │   │   └── ...
│   │   ├── Src/                             # Source code files
│   │   │   ├── button.c                     # Button reading and debouncing logic
│   │   │   ├── dht20.c                      # DHT20 trigger, read and CRC check
│   │   │   ├── fsm_automatic.c              # FSM of the two roads in Normal Mode
│   │   │   ├── fsm_manual.c                 # Mode switching FSM and Manual Mode
│   │   │   ├── fsm_tuning.c                 # FSM of the Red / Amber / Green modification modes
│   │   │   ├── global.c                     # Global variables and default durations
│   │   │   ├── lcd.c                        # LCD 16x2 driver over I2C
│   │   │   ├── led_states.c                 # Set the light state of each road
│   │   │   ├── main.c                       # Initialization, task registration, TIM2 callback
│   │   │   ├── scheduler.c                  # O(1) cooperative scheduler
│   │   │   ├── software_timer.c             # Software timers updated every 10 ms
│   │   │   └── ...
│   │   └── Startup/
│   ├── Drivers/                             # STM32 HAL drivers and CMSIS
│   ├── STM32F103RBTX_FLASH.ld               # Linker script for STM32F103RB
│   ├── Two_road_Traffic_light_system.ioc    # STM32CubeMX configuration file
│   ├── .cproject                            # Eclipse project configuration
│   ├── .mxproject                           # STM32CubeMX project metadata
│   └── .project                             # Eclipse project metadata
│
├── .gitignore
└── README.md                                # You're reading this file
```
Notable Files
main.c: initializes the hardware and the modules, registers the tasks and calls `SCH_Update` in the TIM2 interrupt.
scheduler.c / scheduler.h: O(1) scheduler with one-shot task support.
fsm_manual.c / fsm_automatic.c / fsm_tuning.c: the FSMs for mode switching, automatic, manual and modification modes.
button.c / button.h: button reading and software debouncing.
lcd.c / lcd.h: I2C functions to display the time, mode and sensor values on the 16x2 LCD.
dht20.c / dht20.h: non-blocking temperature and humidity reading.
software_timer.c / software_timer.h: software timers driven by a 10 ms task.
Scheduler Tasks
Task	Period
`button_reading`	10 ms
`timer_run`	10 ms
`fsm_manual_run`	100 ms
`dht20_trigger` (+ `dht20_read` one-shot after 80 ms)	2 s
`toggle_LED`	500 ms
---
6. Mode Descriptions
Normal Mode
Traffic lights of the two roads operate automatically: Red1–Green2 → Red1–Amber2 → Green1–Red2 → Amber1–Red2.
The LCD shows the remaining time of each road, the temperature and the humidity, e.g. `Lane 1:4  T28.5C` / `Lane 2:2  H65%`.
Manual Mode
The lights keep the state they had when entering the mode.
Use the second button to move to the next state. The states follow the same order as Normal Mode, so a road always passes Amber before Red and the two roads are never Green together.
Modify Red Duration
Lights blink at 2 Hz to indicate the editing state.
Use the second button to increase the time (1–99 seconds; after 99 it returns to 1).
Press the third button to confirm and save the new time.
The LCD shows the mode name and the current value.
Modify Amber Duration
Similar behavior to Modify Red Duration, but for the Amber duration.
Modify Green Duration
Similar behavior to Modify Red Duration, but for the Green duration.
Press the first button to cycle through the modes (Normal → Manual → Red → Amber → Green → Normal → ...). Initially, the system runs with 5 seconds for Red, 2 seconds for Amber and 3 seconds for Green. When returning to Normal Mode, the Red duration is set to Green + Amber, and the system restarts the cycle with the new durations.
---
7. How to Build and Run
Clone the Repository
```bash
   git clone https://github.com/khuong2906/Two-road-Traffic-Light-System.git
   cd Two-road-Traffic-Light-System
   ```
Open Project in STM32CubeIDE: `File → Import → General → Existing Projects into Workspace`, then select the `Implementation/` folder.
Check Pin Assignments: ensure the pin assignments in `Two_road_Traffic_light_system.ioc` match the hardware connections in Pin Mapping. The project must be created for the NUCLEO-F103RB board with SYS → Debug = Serial Wire; otherwise the board can be flashed only once.
Build the Project
Compile the code (Ctrl + B).
Flash the Board
Connect the board through the ST-Link USB port and press Run (Ctrl + F11).
Or use STM32CubeProgrammer:
```bash
     STM32_Programmer_CLI -c port=SWD -w Implementation/Debug/Two_road_Traffic_light_system.elf -v -rst
     ```
Verify Operation
Upon start or reset, the system begins in Normal Mode and LED PA5 blinks every 0.5 s.
After about 1 s, the LCD shows the temperature and humidity.
Press Button 1 to cycle modes.
In Manual Mode, press Button 2 to change the light state.
In the modification modes, use Button 2 to increment the time, then press Button 3 to set it.
---
8. Implementing on Hardware
The system is tested on the XNUCLEO-F103RB board with the GROVE expansion board, two traffic light modules, two button modules, the LCD 16x2 and the DHT20 sensor.
<!-- Add a demo GIF or video link here, e.g.
![Demo](./demo.gif)
---
