# Wiring Guide

This document provides a detailed pin-by-pin wiring reference for the RGB Sound Strip project.

---

## Pin Reference Table

### ESP32 to WS2812B LED Strip

| ESP32 Pin | LED Strip Pin | Notes |
|---|---|---|
| GPIO 13 | DATA IN | Through 330-470 ohm resistor |
| External 5V | 5V / VCC | Do NOT use ESP32 5V pin |
| GND | GND | Must share GND with external PSU |

### ESP32 to Sound Sensor (KY-038 / MAX4466)

| ESP32 Pin | Sensor Pin | Notes |
|---|---|---|
| 3.3V | VCC | Do not use 5V — ESP32 ADC is 3.3V max |
| GND | GND | |
| GPIO 34 | AOUT | Analog output — main signal |
| GPIO 35 | DOUT | Digital threshold output — optional |

---

## Power Supply Sizing

The LED strip current draw depends entirely on how many LEDs are on and at what brightness.

| Scenario | Current Draw (approx.) |
|---|---|
| All 60 LEDs, full white, brightness 255 | ~3.6 A |
| All 60 LEDs, full white, brightness 200 | ~2.8 A |
| VU Meter mode, typical music, brightness 200 | ~0.8–1.5 A |
| All LEDs off | ~20 mA |

A 5V 3A power supply is the minimum recommended for a 60-LED strip with BRIGHTNESS_MAX set to 200. If you plan to run full-white effects or use more than 60 LEDs, step up to a 5V 5A supply.

---

## Breadboard Layout Tips

1. Place the 1000 uF capacitor directly across the 5V and GND rails at the end of the strip where power enters. This is the most critical protection step.
2. Run a short jumper from the external PSU GND rail to the ESP32 GND pin so all grounds share a reference point.
3. Keep the data wire as short as practical. Long data wires can pick up noise.
4. If your sound sensor has a sensitivity potentiometer (blue trimmer), adjust it so the DOUT LED just barely lights up in ambient silence. The AOUT signal will then span a useful range.

---

## GPIO Alternatives

If GPIO 13 conflicts with another use (e.g., SD card), you can move the data line to any output-capable GPIO. Update `LED_PIN` in the sketch to match.

If your ESP32 variant does not expose GPIO 34/35, any ADC1 channel pin works: 32, 33, 34, 35, 36, 39. Avoid ADC2 pins (0, 2, 4, 12, 13, 14, 15, 25, 26, 27) when Wi-Fi is active — ADC2 is shared with the Wi-Fi radio and will return incorrect readings.

---

## Sensor Module Notes

### KY-038
- Has both analog (AOUT) and digital (DOUT) outputs.
- Blue potentiometer sets the digital threshold.
- AOUT range on 3.3V supply: approximately 0–3.3V centred around the ambient noise floor.
- Sensitive to high-frequency sounds; adequate for music and clapping.

### MAX4466
- Amplified electret microphone breakout.
- Gain is adjustable via an onboard trimmer.
- Outputs an AC signal centred at VCC/2. With 3.3V supply, the idle voltage is ~1.65V.
- Better frequency response than KY-038; recommended for music visualisation.
- When using MAX4466, ensure the centre-subtract block in the sketch is active.

---

## Schematic (Text Format)

```
[External 5V PSU] ----+---- [LED Strip VCC]
                      |
                   [1000uF]
                      |
[External PSU GND] ---+---- [LED Strip GND] ---- [ESP32 GND]
                                                       |
[ESP32 3.3V] ----> [Sound Sensor VCC]                  |
[ESP32 GND]  ----> [Sound Sensor GND]                  |
[ESP32 GPIO34] <-- [Sound Sensor AOUT]                 |
[ESP32 GPIO35] <-- [Sound Sensor DOUT] (optional)      |
                                                       |
[ESP32 GPIO13] --[330ohm]---> [LED Strip DATA IN]      |
                                                       |
[USB to PC] ---> [ESP32 USB port for programming]      |
```
