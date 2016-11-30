#include "FastLED.h"
#include "frameDefs.h"
#include <NewPing.h>
#include <RunningMedian.h>


#define PIN 6
#define NUM_LEDS 180
#define NUM_FRAMES 30
#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define TRIGGER_DISTANCE 20

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
RunningMedian samples = RunningMedian(1);

short frameIndex = 1;           //what frame is currently being displayed
unsigned long frameExpiration = 0;   //when shoudl that frame expire (triggering the next one)
unsigned long fadeExpiration = 0;

short chaseDelay = 50;         //how many MS should each from be visible for
short fadeDelay = 1;
bool flip = false;
int distance = 0;

CRGB leds[NUM_LEDS];

void blackOut() {
  for (int i=0; i<NUM_LEDS; i++) {
    setPixel(i, 0,0,0);
  }
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
    if (flip == true) {
      frameIndex--;
    } else {
      frameIndex++;
    }
    if (frameIndex > 30) {
      frameIndex = 1;
    }

    if (frameIndex < 0) {
      frameIndex = 30;
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
  //first set some defaults.  These will get overwritten by the distance sensor

  flip = false;
  chaseDelay = 50;


  if (millis() > frameExpiration) {
    distance = sonar.ping_cm();
    if (distance == 0) {
      distance = 200;
    }


    if (distance > 0 && distance <= 20) {
      chaseDelay = 5000;
    }

    if (distance > 20 && distance <= 50) {
      frameIndex = frameIndex - 2;
    }
    
    if (distance > 50) {
      frameIndex = random(0,30);
    }

    Serial.print("Distance: ");
    Serial.println(distance);
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
