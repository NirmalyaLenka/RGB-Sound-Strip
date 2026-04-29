# Components Guide and Bill of Materials

This document lists every part needed to build the RGB Sound Strip, with descriptions to help you identify and purchase them.

---

## Core Components

### ESP32 Development Board

The ESP32 is a dual-core 32-bit microcontroller with built-in Wi-Fi and Bluetooth. For this project, only the CPU and ADC are used — no wireless features are required.

Any standard ESP32 Dev Module with at least 30 GPIO pins will work. Common variants include:
- ESP32 DevKitC V4 (Espressif official)
- ESP32 WROOM-32 Dev Board
- ESP32-D0WDQ6 based 38-pin boards

Look for a board that clearly labels GPIO 34, 35, and 13 on its silkscreen.

Approximate cost: $4–$10 USD

---

### WS2812B Addressable LED Strip

The WS2812B is an individually addressable RGB LED. Each LED contains a red, green, and blue element plus a control chip. The entire strip is controlled from a single data wire.

Specifications to look for:
- Voltage: 5V DC
- LED density: 30, 60, or 144 LEDs per meter
- IP rating: IP30 (bare PCB, for indoor use) or IP65 (silicone coated, water resistant)
- Wire colour code: Red = 5V, White or Green = Data, Black = GND (verify with your specific strip)

A 1-meter, 60-LED/m strip is the standard starting point for this project. You can use longer strips by updating NUM_LEDS in the sketch and upgrading your power supply.

Approximate cost: $5–$15 USD per meter

---

### Sound Sensor Module

Two common options are compatible with this project:

#### KY-038 Sound Sensor Module
- Simple and inexpensive
- Has both analog (AOUT) and digital (DOUT) outputs
- On-board sensitivity trimmer (blue potentiometer)
- Works well for clapping, talking, and general sound detection
- Not ideal for nuanced music because of limited frequency response

#### MAX4466 Electret Microphone Amplifier (Adafruit or compatible)
- Amplified output for better sensitivity
- Adjustable gain via onboard trimmer
- Better frequency response across the audio spectrum
- Preferred for music visualisation projects
- Slightly more expensive

Either module will work. The sketch supports both; the MAX4466 gives better results with music.

Approximate cost: $1–$4 USD (KY-038), $6–$8 USD (MAX4466 breakout)

---

### 5V DC Power Supply

The LED strip requires a dedicated 5V power supply. Never power the strip from the ESP32 USB port — it cannot supply enough current and doing so will damage the board.

Recommended ratings:
- 5V 3A for 30–60 LEDs
- 5V 5A for 60–144 LEDs
- 5V 10A for very long strips or multiple strips

A regulated switching supply with a 5.5mm/2.1mm DC barrel jack is easiest to work with on a breadboard using a compatible adapter.

Approximate cost: $6–$15 USD

---

## Passive Components

### 330–470 Ohm Resistor (x1)

Placed in series on the data line between the ESP32 and the LED strip. It suppresses signal reflections and protects the first LED's input pin from voltage transients. Any carbon film or metal film 1/4W resistor in the 300–500 ohm range is suitable.

### 1000 uF Electrolytic Capacitor (x1)

Placed across the 5V and GND power rails as close to the start of the LED strip as possible. It absorbs inrush current when the strip powers on and stabilises the supply voltage during rapid brightness changes. Use a capacitor rated for at least 6.3V (10V preferred for margin). Observe polarity: the longer leg is positive.

---

## Connectors and Accessories

| Item | Purpose |
|---|---|
| Breadboard (full or half size) | Prototyping and connecting components |
| Male-to-male jumper wires | Breadboard connections |
| Male-to-female jumper wires | Connecting sensor and strip to breadboard |
| Micro-USB or USB-C cable | Programming the ESP32 from your computer |
| DC barrel jack adapter (optional) | Connecting bench power supply to breadboard rails |
| Small flathead screwdriver (optional) | Adjusting sensitivity trimmer on sound sensor |

---

## Estimated Total Cost

| Component | Cost (approx.) |
|---|---|
| ESP32 Dev Board | $6 |
| WS2812B LED Strip (1m, 60 LEDs) | $8 |
| Sound Sensor Module (KY-038) | $2 |
| 5V 3A Power Supply | $8 |
| Resistors (pack) | $1 |
| Capacitors (pack) | $1 |
| Jumper Wires and Breadboard | $4 |
| Total | ~$30 |

Prices vary by supplier and region. AliExpress, Amazon, Adafruit, SparkFun, and local electronics shops all carry these components.

---

## Where to Buy

The following types of suppliers typically carry all components for this project:

- Online general electronics suppliers (search for each component by name or part number)
- Microcontroller-specific stores such as Adafruit or SparkFun
- Local electronics hobby shops
- University or makerspace component libraries (often free for students)

When ordering the LED strip, confirm it is WS2812B and not the older WS2812 (without the B). The WS2812B has better consistency and lower power draw at idle.
