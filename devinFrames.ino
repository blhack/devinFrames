#include "FastLED.h"
#include "frameDefs.h"

#define PIN 6
#define NUM_LEDS 180
#define NUM_FRAMES 30
#define TRIGPIN 8
#define ECHOPIN 9

short frameIndex = 1;           //what frame is currently being displayed
unsigned long frameExpiration = 0;   //when shoudl that frame expire (triggering the next one)
unsigned long fadeExpiration = 0;

short chaseDelay = 50;         //how many MS should each from be visible for
short fadeDelay = 1;
bool flip = false;

CRGB leds[NUM_LEDS];

void blackOut() {
  for (int i=0; i<NUM_LEDS; i++) {
    setPixel(i, 0,0,0);
  }
}

int getDistance() {
  long duration, distance;
  digitalWrite(TRIGPIN, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(TRIGPIN, LOW);
  duration = pulseIn(ECHOPIN, HIGH);
  Serial.println(duration);
  distance = (duration/2) / 29.1;
  return(distance);
}

void frame(int index) {
  blackOut();
  
  short pixelIndex = 0; 
  short pixelStart = 0;
  
  for (int i=index-1; i<180; i+=60) {
    pixelStart = pixelIndex * 3;
    setPixel(i, framePixels[pixelIndex].R,framePixels[pixelIndex].G,framePixels[pixelIndex].B);
    pixelIndex+=2;
  }
  pixelIndex = 1;
  for (int i=60-index; i<180; i+=60){
    pixelStart = pixelIndex * 3;
    setPixel(i, framePixels[pixelIndex].R,framePixels[pixelIndex].G,framePixels[pixelIndex].B);
    pixelIndex+=2;
  }
}

void setPixel(int index, int r, int g, int b) {
  leds[index].r = r;
  leds[index].g = g;
  leds[index].b = b;
}

void expireFrames() {
  if (millis() > frameExpiration) {
    frameIndex++;
    if (frameIndex > 30) {
      frameIndex = 1;
    }
    frameExpiration = millis() + chaseDelay;
  }
}

void expireFade() {
  if (millis() > fadeExpiration) {
    fadeChase();
    fadeExpiration = millis() + fadeDelay;
  }
}

void generateFrame() {
  if (frameIndex == 30 && flip == false) {
    flip = true;
  } else if (frameIndex == 30 && flip == true) {
    flip = false;
  }
}

void fadeChase() {
  for (int i=0; i<6; i++) {
    if (framePixels[i].R > target[i].R) {
      framePixels[i].R = framePixels[i].R - 1;
    }

    if (framePixels[i].R < target[i].R) {
      framePixels[i].R = framePixels[i].R + 1;
    }

    if (framePixels[i].G > target[i].G) {
      framePixels[i].G = framePixels[i].G - 1;
    }

    if (framePixels[i].G < target[i].G) {
      framePixels[i].G = framePixels[i].G + 1;
    }

    if (framePixels[i].B > target[i].B) {
      framePixels[i].B = framePixels[i].B - 1;
    }

    if (framePixels[i].B < target[i].B) {
      framePixels[i].B = framePixels[i].B + 1;
    }  
  }
}

void setup() {
  FastLED.addLeds<NEOPIXEL, PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600);
}

void loop() {
  generateFrame();
  expireFrames();
  expireFade();
  frame(frameIndex);
  FastLED.show();
}
