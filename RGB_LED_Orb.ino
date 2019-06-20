#include "FastLED.h"
#define NUM_LEDS 8
CRGB leds[NUM_LEDS];
#define DATAPIN 5
#define SEGLEN 1

bool changePattern = false;

//https://forum.arduino.cc/index.php?topic=520585.0
// Used for generating interrupts using CLK signal
const int PinA = 3; 
// Used for reading DT signal
const int PinB = 4;   

// Updated by the ISR (Interrupt Service Routine)
volatile int virtualPosition = 0;
volatile int previousTurn = 0;

void isr ()  {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // If interrupts come faster than 5ms, assume it's a bounce and ignore
  if (interruptTime - lastInterruptTime > 5) {
    if (digitalRead(PinB) == LOW)
    {
      virtualPosition++ ;
    }
    else {
      virtualPosition-- ;
    }

    // Keep track of when we were here last (no more than every 5ms)
    lastInterruptTime = interruptTime;
    if(virtualPosition != previousTurn)
    {
      changePattern = true;
      previousTurn = virtualPosition;
    }
  }
}


byte RAINBOW[8][3] = {
  {255, 0, 0},      /*Red*/
  {255, 127, 0},    /*Orange*/
  {255, 255, 0},    /*Yellow*/
  {0, 255, 0},      /*Green*/
  {0, 255, 255},    /*Cyan*/
  {0, 0, 255},      /*Blue*/
  {138, 43, 226},     /*Purple*/
  {128, 0, 125}     /*Violet*/
};

void setup() {
  // Setup LED's
  FastLED.addLeds<WS2811, DATAPIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

  // Rotary pulses are INPUTs
  pinMode(PinA, INPUT);
  pinMode(PinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(PinA), isr, LOW);
  
  Serial.begin (9600);
  Serial.println("Start");
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println(virtualPosition);
  
  if(virtualPosition <= 0)
    virtualPosition = 0; // Prevents value from going negative
    
  if(virtualPosition > 17 )
    virtualPosition = 17; // Caps value for options, increment as more cases are added
  
  switch (virtualPosition) {
    case 1:
      setAll(255, 0, 0);
      break;
    case 2:
      setAll(255, 127, 0);
      break;
    case 3:
      setAll(255, 255, 0);
      break;
    case 4:
      setAll(0, 255, 0);
      break;
    case 5:
      setAll(0, 255, 255);
      break;
    case 6:
      setAll(0, 0, 255);
      break;
    case 7:
      setAll(138, 43, 226);
      break;
    case 8:
      setAll(128, 0, 125);
      break;
    case 9:
      FadeInOut(255, 0, 0);
      break;
    case 10:
      FadeInOut(255, 127, 0);
      break;
    case 11:
      FadeInOut(255, 255, 0);
      break;
    case 12:
      FadeInOut(0, 255, 0);
      break;
    case 13:
      FadeInOut(0, 255, 255);
      break;
    case 14:
      FadeInOut(0, 0, 255);
      break;
    case 15:
      FadeInOut(138, 43, 226);
      break;
    case 16:
      FadeInOut(128, 0, 125);
      break;    
    case 17:
      RainbowHue(20);
      break;
    default:
      FadeInOut(0,0,0);
      break;
  }


}

void FadeInOut(byte red, byte green, byte blue){
  float r, g, b;
 
  for(int k = 0; k < 256; k=k+1) { 
    if (changePattern == true) {
        changePattern = false;
        return;
      }
    
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
    delay(10);
  }
     
  for(int k = 255; k >= 0; k=k-2) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
    delay(10);
  }
}

void RainbowRandom( int SpeedDelay, int ReturnDelay, bool Solid) {
  setAll(0, 0, 0);
  int color = 0;
  for (int i = 0; i < NUM_LEDS; i += 1) {
    if (Solid)
      color = random(8);
    for (int j = 0; j < SEGLEN; j++) {
      if (changePattern == true) {
        changePattern = false;
        return;
      }
      if (!Solid)
        color = random(8);
      setPixel(i + j, RAINBOW[color][0], RAINBOW[color][1], RAINBOW[color][2]);
      setPixel(NUM_LEDS - i - j - 1, RAINBOW[color][0], RAINBOW[color][1], RAINBOW[color][2]);
    }
  }
  showStrip();
  delay(ReturnDelay);
}

void RainbowHue(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      if (changePattern == true) {
        changePattern = false;
        return;
      }
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }
    showStrip();
    delay(SpeedDelay);
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
  
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  FastLED.show();
#endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
#endif
}
