# ESP32-S3 LED Reaction Game

A small reaction-time game built in C++ with ESP-IDF. Three LEDs and a buzzer count down to a randomly delayed **GO** signal. Press the button as quickly as possible after GO; pressing early triggers the fail sequence.

## Functional Demo

https://youtu.be/c_4FXjEqZLM

## Hardware

- ESP32-S3 development board
- 3 LEDs with current-limiting resistors
- Push button
- Buzzer
- Breadboard and jumper wires

## Pin assignments

| Component | ESP32-S3 GPIO |
|---|---:|
| Ready LED | GPIO 5 |
| Set LED | GPIO 6 |
| Go LED | GPIO 15 |
| Buzzer | GPIO 11 |
| Button | GPIO 8 |

The button uses the ESP32-S3's internal pull-down resistor, so connect it between GPIO 8 and 3.3 V. Connect each LED through its own resistor.

## How it works

1. The LEDs blink while the game waits to begin.
2. Press the button to start the READY/SET sequence.
3. After a random 1–5 second delay, the GO LED turns on.
4. Press the button; the reaction time is printed in milliseconds.
5. Pressing before GO activates the fail sequence.

The program is organized as a finite-state machine with `WAIT`, `PREPARE`, `START`, and `FAIL` states.

## Build and flash

Open an ESP-IDF terminal in the project directory and run:

```powershell
idf.py set-target esp32s3
idf.py build
idf.py flash monitor
```

Press `Ctrl+]` to exit the serial monitor.

## Built with

- ESP-IDF
- C++
- FreeRTOS timing and GPIO APIs
