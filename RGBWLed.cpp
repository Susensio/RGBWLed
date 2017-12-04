/*********************************
 * RGBWLed library for RGB+W leds
 * Implements HSI colour space
 *
 * Created by Saul Nogueras Ona
 * February 28, 2015
 ********************************/

#include "Arduino.h"
#include "RGBWLed.h"



// Sets LED pins as output and initializes variables
RGBWLed::RGBWLed(byte redPin, byte greenPin, byte bluePin, byte whitePin) {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(whitePin, OUTPUT);

  rPin = redPin;
  gPin = greenPin;
  bPin = bluePin;
  wPin = whitePin;
  
  globalIntensity = 255;
  
  fadingHSI = false;
  fadingKelvin = false;
  aux1 = 0;
  aux2 = 0;
  aux3 = 0;
  intensity = 0;
}

void RGBWLed::setIntensity(byte value) {
	globalIntensity = pgm_read_byte(&curve[value]);		// Dim curve correction
}


// Writes RGB values into the PWM register
void RGBWLed::writeRGBW(RGBW colour) {
	writeRGBW( colour.r, colour.g, colour.b, colour.w );
}
void RGBWLed::writeRGBW(byte red, byte green, byte blue, byte white) {
	if(globalIntensity != 255){
		red = (int)red*globalIntensity/255;
		green = (int)green*globalIntensity/255;
		blue = (int)blue*globalIntensity/255;
		white = (int)white*globalIntensity/255;
	}
  analogWrite(rPin, red);
  analogWrite(gPin, green);
  analogWrite(bPin, blue);
  analogWrite(wPin, white);

/*
  Serial.print(globalIntensity);
  Serial.print("\t");
  Serial.print(red);
  Serial.print("\t");
  Serial.print(green);
  Serial.print("\t");
  Serial.print(blue);
  Serial.print("\t");
  Serial.println(white);
 */
}


// Outputs a RGBW colour using the dim curve correction (overloaded method)
void RGBWLed::setRGBW(byte red, byte green, byte blue, byte white) {
  writeRGBW( pgm_read_byte(&curve[red]), pgm_read_byte(&curve[green]), pgm_read_byte(&curve[blue]), pgm_read_byte(&curve[white]) );
}
void RGBWLed::setRGBW(RGBW colour) {
  setRGBW( colour.r, colour.g, colour.b, colour.w );
}


// Outputs a HSI colour (overloaded method)
void RGBWLed::setHSI(float hue, float sat, byte in) {
  HSI colour = {hue, sat, in};
  setHSI(colour);
}
void RGBWLed::setHSI(HSI colour) {
  RGBW res = HSItoRGBW(colour);

  writeRGBW( res.r, res.g, res.b, res.w );
}


// Outputs a Kelvin colour (overloaded method)
void RGBWLed::setKelvin(float temp, byte in) {
  Kelvin colour = {temp, in};
  setKelvin(colour);
}
void RGBWLed::setKelvin(Kelvin colour) {
  RGBW res = KtoRGBW(colour);

  writeRGBW( res.r, res.g, res.b, res.w );
}



// Converts HSI to RGBW colours.
RGBW RGBWLed::HSItoRGBW(HSI colour) {
  RGBW res;

  colour.h = fmod(colour.h, 360);					// [0..359] degrees
  if (colour.h < 0) colour.h += 360;
  colour.s = constrain(colour.s, 0, 1);			// [0..1]
  colour.i = pgm_read_byte(&curve[colour.i]);		// Dim curve correction

  float red;
  float green;
  float blue;
  float white;

  /*
   * Equations to Convert HSI Values to RGBW Values
   */
  switch ( (int)colour.h / 120) {

    case 0: {		// Hue: 0..119
        float cos_num = cos( degToRad(colour.h) );
        float cos_den = cos( degToRad(60 - colour.h) );

        red = colour.s * colour.i * ( 1 + cos_num / cos_den ) / 3;
        green = colour.s * colour.i * ( 1 + ( 1 - cos_num / cos_den ) ) / 3;
        blue = 0;
        white = ( 1 - colour.s ) * colour.i;
        break;
      }

    case 1: {		// Hue: 120..239
        float cos_num = cos( degToRad(colour.h - 120) );
        float cos_den = cos( degToRad(180 - colour.h) );

        red = 0;
        green = colour.s * colour.i * ( 1 + cos_num / cos_den ) / 3;
        blue = colour.s * colour.i * ( 1 + ( 1 - cos_num / cos_den ) ) / 3;
        white = ( 1 - colour.s ) * colour.i;
        break;
      }

    case 2: {		// Hue: 240..359
        float cos_num = cos( degToRad(colour.h - 240) );
        float cos_den = cos( degToRad(300 - colour.h) );

        red = colour.s * colour.i * ( 1 + ( 1 - cos_num / cos_den ) ) / 3;
        green = 0;
        blue = colour.s * colour.i * ( 1 + cos_num / cos_den ) / 3;
        white = ( 1 - colour.s ) * colour.i;
        break;
      }
  }

  // Constrains values for 8 bit PWM
  res.r = (byte)(constrain(red, 0, 255) + 0.5);
  res.g = (byte)(constrain(green, 0, 255) + 0.5);
  res.b = (byte)(constrain(blue, 0, 255) + 0.5);
  res.w = (byte)(constrain(white, 0, 255) + 0.5);
  return res;
}


// Converts Kelvin to RGB colours.
RGBW RGBWLed::KtoRGBW(Kelvin colour) {
  RGBW res;
  colour.t = constrain(colour.t, 500, 40000);		// [1000..40000] Kelvin
  colour.i = pgm_read_byte(&curve[colour.i]);		// Dim curve correction
  float red;
  float green;
  float blue;
  float white;

  float temp = colour.t / 100;

  // Calculate red
  if (temp <= 66) red = 255;
  else red = 329.698727446 * pow( temp - 60 , -0.1332047592 );

  // Calculate green
  if (temp < 66) green = 99.4708025861 * log(temp) - 161.1195681661 ;
  else green = 288.1221695283 * pow( temp - 60 , -0.0755148492 );

  // Calculate blue
  if (temp <= 19) blue = 0;
  else if (temp <= 66) blue = 138.5177312231 * log(temp - 10) - 305.0447927307;
  else blue = 255;

  red = red * colour.i / 255;
  green = green * colour.i / 255;
  blue = blue * colour.i / 255;
  float m = fmin( red, fmin(green, blue) );

  // Constrains values for 8 bit PWM
  res.r = (byte)(constrain(red - m, 0, 255) + 0.5);
  res.g = (byte)(constrain(green - m, 0, 255) + 0.5);
  res.b = (byte)(constrain(blue - m, 0, 255) + 0.5);
  res.w = (byte)(constrain(m, 0, 255) + 0.5);
  return res;
}


// Converts Kelvin to HSI colour. 
HSI RGBWLed::KtoHSI(Kelvin colour){
  RGBW colourRGBW = KtoRGBW(colour);
  return RGBWtoHSI(colourRGBW);
}


// Converts RGBW to HSI colour. 
HSI RGBWLed::RGBWtoHSI(RGBW colour){
  HSI res;
  float R = colour.r + colour.w/3.0;
  float G = colour.g + colour.w/3.0;
  float B = colour.b + colour.w/3.0;
	
  float		hue;
  float		sat;
  float		in = ( R + G + B )/3.0;
  
  if (in == 0) sat = 0;
  else {
	float m = fmin(R, fmin(G, B));
	sat = 1 - m/in;
  }

  if ( colour.r==colour.g && colour.g==colour.b ) hue = 0;
  else {
    float aux = ( R - G/2 - B/2 ) / sqrt( R*R + G*G + B*B - R*G - R*B - G*B );
    if ( G >= B ) hue = radToDeg( acos(aux) );
    else hue = 360 - radToDeg( acos(aux) );
  }
  
  in = 255*pow( 3*in/255 , 1.0/2.8 ) +0.5;				// Inverse dim curve correction
  in = fmin(in,255);
  res = { hue , sat , (byte)in };
  return res;
}
  

// Starts a HSI transition.
void RGBWLed::fadeHSI(HSI initcolour, HSI endcolour, unsigned int duration_s, unsigned int steps) {
  fadingHSI = true;
  fadingKelvin = false;
  period = (unsigned long)duration_s * 1000 / steps;		// Interval in milliseconds
  tlast = millis();
  step = steps;

  aux1 = (endcolour.h - initcolour.h) / steps;
  aux2 = (endcolour.s - initcolour.s) / steps;
  aux3 = ((float)endcolour.i - initcolour.i) / steps;
  intensity = (float)initcolour.i;

  colourH = initcolour;
  setHSI( colourH );
}


// Starts a Kelvin transition.
void RGBWLed::fadeKelvin(Kelvin initcolour, Kelvin endcolour, unsigned int duration_s, unsigned int steps) {
  fadingHSI = false;
  fadingKelvin = true;
  period = (unsigned long)duration_s * 1000 / steps;		// Interval in milliseconds
  tlast = millis();
  step = steps;
  
  aux1 = (endcolour.t - initcolour.t) / steps;
  aux3 = ((float)endcolour.i - initcolour.i) / steps;
  intensity = (float)initcolour.i;

  colourK = initcolour;
  setKelvin( colourK );
}

// Update fade process
void RGBWLed::updateFade(void) {
  if ( isFading() ) {
    if ( step ) {									// Still fading?
      unsigned long tnow = millis();
      if ( ( tnow - tlast) >= period ) {			// Next step?
        tlast += period;
        intensity += aux3;
		
        if ( fadingHSI )							// HSI fading
          colourH = { colourH.h + aux1 , colourH.s + aux2 , (byte)intensity };
        else	  									// Kelvin fading
          colourK = { colourK.t + aux1 , (byte)intensity };
        
        step--;											// 1 less step to go!
      }
	  
	  if ( fadingHSI )
	    setHSI( colourH );
	  else
	    setKelvin( colourK );
	
    } else {
      fadingHSI = false;
      fadingKelvin = false;
    }
  }
}

// Save fading parameters for setting up an alternative fade
void RGBWLed::pauseFade(void){
    periodCopy = period;
    tlastCopy = tlast;
    stepCopy = step;
    fadingHSICopy = fadingHSI;
    fadingKelvinCopy = fadingKelvin;
    colourHCopy = colourH;
    colourKCopy = colourK;
    aux1Copy = aux1;
    aux2Copy = aux2;
    aux3Copy = aux3;
    intensityCopy = intensity;
	
	fadingHSI = false;
    fadingKelvin = false;
}

// Load saved fading parameters for continuing previous fade
void RGBWLed::resumeFade(void){
    period = periodCopy;
    tlast = tlastCopy;
    step = stepCopy;
    fadingHSI = fadingHSICopy;
    fadingKelvin = fadingKelvinCopy;
    colourH = colourHCopy;
    colourK = colourKCopy;
    aux1 = aux1Copy;
    aux2 = aux2Copy;
    aux3 = aux3Copy;
    intensity = intensityCopy;
}


boolean RGBWLed::isFading(void) {
  return fadingHSI || fadingKelvin;
}

float RGBWLed::degToRad(float deg) {
  return deg * 3.14159 / 180;
}
float RGBWLed::radToDeg(float rad) {
  return rad * 180 / 3.14159;
}