/*
 * RGB Sound Strip - ESP32 Controller
 * ------------------------------------
 * Controls a WS2812B/NeoPixel LED strip based on sound input from a
 * KY-038 or MAX4466 microphone/sound sensor module.
 *
 * Author  : RGB Sound Strip Project
 * Board   : ESP32 Dev Module
 * Version : 1.0.0
 *
 * Libraries required:
 *   - FastLED (v3.6.0 or newer) — https://github.com/FastLED/FastLED
 *
 * Wiring Summary:
 *   Sound Sensor AOUT  -> GPIO 34 (ADC1_CH6, input only)
 *   Sound Sensor DOUT  -> GPIO 35 (optional digital threshold)
 *   Sound Sensor VCC   -> 3.3V
 *   Sound Sensor GND   -> GND
 *   LED Strip DATA     -> GPIO 13
 *   LED Strip VCC      -> 5V (external power supply recommended)
 *   LED Strip GND      -> GND (shared with ESP32 GND)
 */

#include <FastLED.h>

// ---------------------------------------------------------------------------
// Configuration — edit these to match your hardware
// ---------------------------------------------------------------------------

#define LED_PIN          13      // Data pin connected to the LED strip
#define NUM_LEDS         60      // Number of LEDs on your strip
#define LED_TYPE         WS2812B // LED chip type (WS2812B, WS2811, SK6812, etc.)
#define COLOR_ORDER      GRB     // Color byte order for your strip

#define SOUND_PIN        34      // Analog pin connected to sound sensor AOUT
#define SOUND_DPIN       35      // Digital pin connected to sound sensor DOUT (optional)

#define BRIGHTNESS_MAX   200     // Maximum LED brightness (0–255); lower = less heat
#define BRIGHTNESS_MIN   10      // Minimum LED brightness when quiet

#define SAMPLE_COUNT     30      // Number of ADC samples averaged per cycle
#define NOISE_FLOOR      150     // ADC values below this are treated as silence
#define SOUND_CEILING    3000    // ADC value considered maximum sound level

#define SMOOTHING        0.2f    // EMA smoothing factor (0.0 = very smooth, 1.0 = raw)

// ---------------------------------------------------------------------------
// Mode definitions
// ---------------------------------------------------------------------------

enum VisualMode {
  MODE_VU_METER   = 0,  // Classic left-to-right volume meter
  MODE_PULSE      = 1,  // All LEDs pulse with sound intensity
  MODE_RAINBOW    = 2,  // Rainbow that shifts speed with sound
  MODE_FIRE       = 3,  // Fire simulation driven by sound
  MODE_COUNT      = 4
};

// Change this to select a mode, or wire a button to cycle through them
VisualMode currentMode = MODE_VU_METER;

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------

CRGB leds[NUM_LEDS];

float smoothedLevel  = 0.0f;  // Smoothed sound level (0.0 – 1.0)
float peakLevel      = 0.0f;  // Decaying peak hold value
uint8_t hueOffset    = 0;     // Global hue shift for rainbow effects
uint8_t heat[NUM_LEDS];       // Heat array for fire simulation

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(500);

  // Initialise FastLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS_MAX);
  FastLED.clear();
  FastLED.show();

  // ADC configuration for ESP32
  analogReadResolution(12);        // 12-bit ADC → 0–4095
  analogSetAttenuation(ADC_11db);  // Full 0–3.3 V range

  // Initialise heat array for fire mode
  memset(heat, 0, sizeof(heat));

  Serial.println("RGB Sound Strip ready.");
  Serial.print("Mode: ");
  Serial.println(currentMode);
}

// ---------------------------------------------------------------------------
// Helper: read and average multiple ADC samples
// ---------------------------------------------------------------------------

uint16_t readSoundLevel() {
  uint32_t sum = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    sum += analogRead(SOUND_PIN);
    delayMicroseconds(200);
  }
  uint16_t avg = sum / SAMPLE_COUNT;

  // Centre-subtract if using a microphone (signal swings around midpoint ~2048)
  // For peak-detection mics (KY-038) this is not needed; comment out if unsure
  if (avg > 2048) avg = avg - 2048;
  else            avg = 2048 - avg;

  return avg;
}

// ---------------------------------------------------------------------------
// Helper: map sound level to normalised float 0.0–1.0
// ---------------------------------------------------------------------------

float normalisedLevel(uint16_t raw) {
  if (raw < NOISE_FLOOR) return 0.0f;
  float norm = (float)(raw - NOISE_FLOOR) / (float)(SOUND_CEILING - NOISE_FLOOR);
  return constrain(norm, 0.0f, 1.0f);
}

// ---------------------------------------------------------------------------
// Visual mode: VU Meter
// ---------------------------------------------------------------------------

void renderVuMeter(float level) {
  int litCount = (int)(level * NUM_LEDS);

  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < litCount) {
      // Green → Yellow → Red gradient
      uint8_t hue = map(i, 0, NUM_LEDS - 1, 96, 0); // 96=green, 0=red in HSV
      leds[i] = CHSV(hue, 255, 255);
    } else {
      leds[i] = CRGB::Black;
    }
  }

  // Peak hold indicator
  int peakIdx = (int)(peakLevel * (NUM_LEDS - 1));
  if (peakIdx >= 0 && peakIdx < NUM_LEDS) {
    leds[peakIdx] = CRGB::White;
  }
}

// ---------------------------------------------------------------------------
// Visual mode: Pulse
// ---------------------------------------------------------------------------

void renderPulse(float level) {
  uint8_t brightness = (uint8_t)(BRIGHTNESS_MIN + level * (BRIGHTNESS_MAX - BRIGHTNESS_MIN));
  uint8_t hue = hueOffset;

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, 230, brightness);
  }
  hueOffset += 1; // Slow hue drift even when quiet
}

// ---------------------------------------------------------------------------
// Visual mode: Rainbow
// ---------------------------------------------------------------------------

void renderRainbow(float level) {
  // Sound drives how fast the rainbow scrolls
  uint8_t speed = (uint8_t)(2 + level * 30);
  hueOffset += speed;

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hueOffset + (i * 256 / NUM_LEDS), 255, BRIGHTNESS_MAX);
  }
}

// ---------------------------------------------------------------------------
// Visual mode: Fire (sound fans the flames)
// ---------------------------------------------------------------------------

void renderFire(float level) {
  // Sound adds heat at the base
  uint8_t baseHeat = (uint8_t)(80 + level * 175);

  // Randomly ignite sparks near the bottom
  for (int i = 0; i < 3; i++) {
    int pos = random8(4);
    heat[pos] = qadd8(heat[pos], random8(baseHeat / 2, baseHeat));
  }

  // Cool down every cell
  for (int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8(heat[i], random8(0, 25));
  }

  // Drift heat upward
  for (int i = NUM_LEDS - 1; i >= 2; i--) {
    heat[i] = (heat[i - 1] + heat[i - 2] + heat[i - 2]) / 3;
  }

  // Convert heat to colour and display
  for (int i = 0; i < NUM_LEDS; i++) {
    CRGB color = HeatColor(heat[i]);
    leds[i] = color;
  }
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------

void loop() {
  uint16_t raw     = readSoundLevel();
  float    current = normalisedLevel(raw);

  // Exponential moving average smoothing
  smoothedLevel = smoothedLevel + SMOOTHING * (current - smoothedLevel);

  // Peak hold with slow decay
  if (smoothedLevel > peakLevel) {
    peakLevel = smoothedLevel;
  } else {
    peakLevel *= 0.97f; // Decay rate — increase for faster fall
  }

  // Render selected mode
  switch (currentMode) {
    case MODE_VU_METER: renderVuMeter(smoothedLevel); break;
    case MODE_PULSE:    renderPulse(smoothedLevel);   break;
    case MODE_RAINBOW:  renderRainbow(smoothedLevel); break;
    case MODE_FIRE:     renderFire(smoothedLevel);    break;
    default:            renderVuMeter(smoothedLevel); break;
  }

  FastLED.show();

  // Debug output to Serial Monitor (9600 baud or 115200 baud)
  Serial.print("RAW: ");
  Serial.print(raw);
  Serial.print("  Level: ");
  Serial.print(smoothedLevel, 3);
  Serial.print("  Peak: ");
  Serial.println(peakLevel, 3);

  delay(10); // ~60–80 fps effective refresh
}
