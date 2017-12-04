# RGBWLed library for Arduino

Arduino library for interfacing with RGB+W leds. Implements [HSI colorspace](https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSI) and [Color temperature](https://en.wikipedia.org/wiki/Color_temperature) besides raw RGBW manipulation. Allows smooth **non blocking** fading between colors in all colorspaces. Corrects for non-linear brightness when using PWM.

Fading interpolation in Color temperature between two kelvin values results in natural colors, perfect for representing sunrise and sunset effects.
![Color temperature scale](http://cdn3.craftsy.com/blog/wp-content/uploads/2014/08/color-temp.jpg "Color temperature scale")



## Installation

1. Download the library from [GitHub repository](https://github.com/Susensio/RGBWLed/archive/master.zip)
2. Open Arduino IDE
3. Go to Sketch > Include Library > Add .ZIP Library...
4. Search for the downloaded file


## Usage

Include the library at the start of your code and initialize pins as OUTPUT
```arduino
#include <RGBWLed.h>

RGBWLed (byte redPin, byte greenPin, byte bluePin, byte whitePin);
```

Simple functions to change the led color 
```arduino
// red, green and blue are [0..255] values, raw values
void writeRGBW(byte red, byte green, byte blue, byte white);

// red, green and blue are [0..255] values, dim correction
void setRGBW(byte red, byte green, byte blue, byte white);

// hue in degrees, saturation normalized [0..1] and intensity 8-bits [0..255]
void setHSI(float hue, float sat, byte in);

// temp in Kelvin, intensity 8-bits [0..255]
void setKelvin(float temp, byte in);
```


Asynchronus functions, `void updateFade();` is needed to change led output
```arduino
// Sets global intensity
void setIntensity(byte value);

// Fade functions, duration in seconds
void fadeHSI(HSI initcolour, HSI endcolour, unsigned int duration_s, unsigned int steps);
void fadeKelvin(Kelvin initcolour, Kelvin endcolour, unsigned int duration_s, unsigned int steps);

// Needs to be called periodically to update fade progress and apply global intensity
void updateFade(void);

// Helper functions
boolean isFading(void);
void pauseFade(void);
void resumeFade(void);

```



## Credits

* HSI conversions
Thanks to **Brian Neltner** from [saikoled.com](http://blog.saikoled.com/post/44677718712/how-to-convert-from-hsi-to-rgb-white)

* Kelvin conversions
Thanks to **Tanner Helland** from [tannerhelland.com](http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/)
