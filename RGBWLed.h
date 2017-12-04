/*********************************
 * RGBWLed library for RGB+W leds
 * Implements HSI colour space
 *
 * Created by Saul Nogueras Ona
 * February 28, 2015
 ********************************/

#ifndef RGBWLed_h
#define RGBWLed_h
#include "Arduino.h"

/**
 * Helper class for RGBW-colours.
 */
typedef struct {
  byte 		r;
  byte 		g;
  byte		b;
  byte 		w;
} RGBW;

/**
 * Helper class for HSI-colours.
 */
typedef struct {
  float		h;
  float		s;
  byte		i;
} HSI;

/**
 * Helper class for Kelvin colours.
 */
typedef struct {
  float		t;
  byte		i;
} Kelvin;

/*
 * Exponential lookup table for dimming
 * Gamma = 2.8
 */
const PROGMEM byte curve[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };


class RGBWLed {
  public:
    // Initializes an instance and configures LED pins as OUTPUTs
    RGBWLed (byte redPin, byte greenPin, byte bluePin, byte whitePin);

  	// Sets global intensity
  	void setIntensity(byte value);
  	
  	// red, green and blue are [0..255] values, raw values
    void writeRGBW(byte red, byte green, byte blue, byte white);
    void writeRGBW(RGBW colour);
	
    // red, green and blue are [0..255] values, dim correction
    void setRGBW(byte red, byte green, byte blue, byte white);
    void setRGBW(RGBW colour);

    // hue in degrees, saturation normalized [0..1] and intensity 8-bits [0..255]
    void setHSI(float hue, float sat, byte in);
    void setHSI(HSI colour);

    // temp in Kelvin, intensity 8-bits [0..255]
    void setKelvin(float temp, byte in);
    void setKelvin(Kelvin colour);

    // Fade functions, duration in seconds
    void fadeHSI(HSI initcolour, HSI endcolour, unsigned int duration_s, unsigned int steps);
    void fadeKelvin(Kelvin initcolour, Kelvin endcolour, unsigned int duration_s, unsigned int steps);
    void updateFade(void);

    boolean isFading(void);
  	void pauseFade(void);
  	void resumeFade(void);
  	
  	// This function allows fading in HSI colourspace between Kelvin colours
  	static HSI KtoHSI(Kelvin colour);
	
	
  private:
    byte rPin;
    byte gPin;
    byte bPin;
    byte wPin;
	
    byte globalIntensity;

	// Variables for fading
    unsigned long period;
    unsigned long tlast;
    unsigned int step;
    boolean fadingHSI;
    boolean fadingKelvin;
    HSI colourH;
    Kelvin colourK;
    float aux1;
    float aux2;
    float aux3;
    float intensity;
	
	// Variables for pause fading
    unsigned long periodCopy;
    unsigned long tlastCopy;
    unsigned int stepCopy;
    boolean fadingHSICopy;
    boolean fadingKelvinCopy;
    HSI colourHCopy;
    Kelvin colourKCopy;
    float aux1Copy;
    float aux2Copy;
    float aux3Copy;
    float intensityCopy;
	
	// Helpers
    static RGBW HSItoRGBW(HSI colour);
    static RGBW KtoRGBW(Kelvin colour);
	  static HSI RGBWtoHSI(RGBW colour);
    static float degToRad(float deg);
	  static float radToDeg(float rad);
};

#endif