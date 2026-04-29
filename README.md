# RGB Sound Strip

A beginner-friendly ESP32 project that drives a WS2812B addressable LED strip in real time using audio input from a microphone sound sensor. Sound levels are sampled, smoothed, and mapped to four selectable visual modes: VU Meter, Pulse, Rainbow, and Fire.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Wiring Diagram](#wiring-diagram)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Configuration](#configuration)
- [Visual Modes](#visual-modes)
- [Serial Debug Output](#serial-debug-output)
- [Troubleshooting](#troubleshooting)
- [Project Structure](#project-structure)
- [License](#license)

---

## Overview

This project uses an ESP32 microcontroller to read audio amplitude from an analog sound sensor and translate it into dynamic LED animations on an addressable RGB strip. The firmware is written in Arduino C++ and relies on the FastLED library for high-performance LED control.

The demo page (`demo/index.html`) lets you preview all four visual modes in a browser before you build the physical circuit.

---

## Features

- Four distinct real-time visual modes
- Exponential moving average (EMA) smoothing to eliminate flicker
- Peak hold with configurable decay for a professional VU feel
- All key parameters configurable from a single section at the top of the source file
- Clean serial debug output for calibration
- No cloud, no Wi-Fi required — runs fully offline on the ESP32

---

## Hardware Requirements

| Component | Specification | Quantity |
|---|---|---|
| Microcontroller | ESP32 Dev Module (38-pin or 30-pin) | 1 |
| Addressable LED Strip | WS2812B, 5V, 60 LEDs/m (or any density) | 1 m (min) |
| Sound Sensor Module | KY-038 or MAX4466 breakout | 1 |
| Power Supply | 5V DC, minimum 3A (for 60 LEDs at full white) | 1 |
| Resistor | 330-470 ohm, 1/4W | 1 |
| Capacitor | 1000 uF, 6.3V or higher (electrolytic) | 1 |
| Jumper Wires | Male-to-male and male-to-female | assorted |
| Breadboard | Full-size or half-size | 1 |
| USB Cable | Micro-USB or USB-C (matching your ESP32 board) | 1 |

---

## Wiring Diagram

```
                   +5V (External PSU)
                        |
                       [C1 1000uF]   <- place as close to strip as possible
                        |
ESP32                  GND
---------
GPIO 13  ---[R 330ohm]----> LED Strip DATA IN
3.3V     <not used for strip>
GND      --------------------> LED Strip GND (also connect to PSU GND)

Sound Sensor (KY-038 / MAX4466)
--------------------------------
VCC      ---> ESP32 3.3V
GND      ---> ESP32 GND
AOUT     ---> ESP32 GPIO 34
DOUT     ---> ESP32 GPIO 35  (optional; used only for digital threshold)
```

### Important Notes

1. Never power the LED strip from the ESP32 3.3V or 5V pins. Use a dedicated 5V supply rated for at least 60 mA per LED at full brightness.
2. Always connect the GND of the external supply to the ESP32 GND to share a common reference.
3. The 330-ohm resistor on the data line protects against ringing and signal overshoot.
4. The 1000 uF capacitor across the strip power rails absorbs inrush current on startup.
5. GPIO 34 and 35 on the ESP32 are input-only pins — ideal for ADC without accidental output conflicts.

---

## Software Requirements

| Software | Version | Notes |
|---|---|---|
| Arduino IDE | 2.x recommended | arduino.cc/en/software |
| ESP32 Board Package | 2.0.0 or newer | Install via Board Manager |
| FastLED Library | 3.6.0 or newer | Install via Library Manager |

---

## Installation

### Step 1 — Install Arduino IDE

Download and install Arduino IDE 2.x from https://www.arduino.cc/en/software.

### Step 2 — Add ESP32 Board Support

1. Open Arduino IDE.
2. Go to File > Preferences.
3. In the "Additional boards manager URLs" field, add:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

4. Go to Tools > Board > Boards Manager.
5. Search for "esp32" and install the package by Espressif Systems.

### Step 3 — Install FastLED

1. Go to Sketch > Include Library > Manage Libraries.
2. Search for "FastLED".
3. Install the library by Daniel Garcia (version 3.6.0 or newer).

### Step 4 — Open the Sketch

1. Clone or download this repository.
2. Open `src/rgb_sound_strip.ino` in Arduino IDE.

### Step 5 — Select Your Board

1. Go to Tools > Board > ESP32 Arduino > ESP32 Dev Module.
2. Set the upload speed to 921600 for faster uploads.
3. Select the correct COM port under Tools > Port.

### Step 6 — Configure and Upload

Edit the configuration section at the top of the sketch (see Configuration below), then click the Upload button.

---

## Configuration

All user-configurable parameters are located at the top of `src/rgb_sound_strip.ino` under the comment block labelled "Configuration".

```cpp
#define LED_PIN          13      // GPIO pin connected to LED strip data line
#define NUM_LEDS         60      // Total number of LEDs on your strip
#define LED_TYPE         WS2812B // Chip type: WS2812B, WS2811, SK6812, etc.
#define COLOR_ORDER      GRB     // GRB for WS2812B; RGB for WS2811

#define SOUND_PIN        34      // Analog pin connected to sound sensor AOUT
#define BRIGHTNESS_MAX   200     // 0-255; reduce if LEDs run hot or PSU is small
#define BRIGHTNESS_MIN   10      // Minimum brightness when quiet

#define SAMPLE_COUNT     30      // Higher = smoother but slower response
#define NOISE_FLOOR      150     // Raise if you see LEDs flickering in silence
#define SOUND_CEILING    3000    // Lower if loud sounds do not fill the strip
#define SMOOTHING        0.2f    // EMA coefficient; 0.05 = very smooth, 0.8 = reactive
```

To change the visual mode, set `currentMode` in the globals section:

```cpp
VisualMode currentMode = MODE_VU_METER;  // Change to MODE_PULSE, MODE_RAINBOW, or MODE_FIRE
```

---

## Visual Modes

### MODE_VU_METER
A classic volume-unit bar that grows from left to right. LEDs transition from green at the quiet end to red at the loud end. A white peak-hold dot decays slowly after each transient.

### MODE_PULSE
All LEDs pulse simultaneously in brightness. The hue drifts slowly over time so the colour shifts even in a quiet room. Loud sounds increase brightness sharply.

### MODE_RAINBOW
A full rainbow scrolls along the strip. Loud sounds accelerate the scroll speed; silence slows it to a gentle drift.

### MODE_FIRE
A fire simulation driven by heat diffusion. Sound increases the heat injected at the base, producing taller and more aggressive flames.

---

## Serial Debug Output

Open the Serial Monitor at 115200 baud to see real-time values:

```
RAW: 312  Level: 0.082  Peak: 0.145
RAW: 891  Level: 0.291  Peak: 0.312
RAW: 2140 Level: 0.734  Peak: 0.734
```

Use these values to calibrate `NOISE_FLOOR` and `SOUND_CEILING` for your environment. In a quiet room, RAW should read below your NOISE_FLOOR. At the loudest expected sound, RAW should approach SOUND_CEILING.

---

## Troubleshooting

| Symptom | Likely Cause | Fix |
|---|---|---|
| LEDs do not light up | Wiring error or wrong LED_PIN | Check data line and resistor |
| Colours look wrong | Incorrect COLOR_ORDER | Try RGB, GRB, or BGR in config |
| LEDs flicker randomly | PSU too weak or missing capacitor | Add 1000 uF cap; use stronger PSU |
| No response to sound | Wrong SOUND_PIN or bad sensor | Verify GPIO 34 reads values in Serial Monitor |
| LEDs always fully lit | NOISE_FLOOR too low | Raise NOISE_FLOOR until idle reads 0 |
| Weak response to loud sound | SOUND_CEILING too high | Lower SOUND_CEILING to 1500-2000 |
| Upload fails | Wrong board or COM port selected | Recheck Tools > Board and Tools > Port |
| Serial Monitor shows 0 only | Centre-subtraction sign issue | Comment out the centre-subtract block |

---

## Project Structure

```
rgb-sound-strip/
|
|-- src/
|   |-- rgb_sound_strip.ino      Main firmware sketch
|
|-- demo/
|   |-- index.html               Interactive browser demo of all visual modes
|
|-- docs/
|   |-- wiring.md                Detailed wiring notes and pin reference
|   |-- components.md            Full bill of materials with purchase links guidance
|
|-- .gitignore                   Arduino and OS build artefacts
|-- LICENSE                      MIT License
|-- README.md                    This file
```

---

## License

This project is released under the MIT License. See the LICENSE file for full terms. You are free to use, modify, and distribute this project for personal and commercial purposes with attribution.
