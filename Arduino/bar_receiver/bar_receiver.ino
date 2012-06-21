#include "HL1606stripPWM.h"

#define NUM_COLS 8
#define NUM_ROWS 8
#define NUM_UNITS 64
#define LATCH_PIN 10

HL1606stripPWM strip = HL1606stripPWM(NUM_UNITS, LATCH_PIN); 
char serialBytes[NUM_COLS];

int red=255,green=255,blue=255;

void setup() {
  Serial.begin(9600);
  Serial.println("Started");
  configureStrip();
}

void loop() {
  
  int walk = 50;
  red += random(walk) - random(walk);
  green += random(walk) - random(walk);
  blue += random(walk) - random(walk);
  red = constrain(red,0,255);
  green = constrain(green,0,255);
  blue = constrain(blue,0,255);
  
  if(Serial.available() > 0){
    int bytesRead = Serial.readBytesUntil('\n', serialBytes, NUM_COLS);
    if(bytesRead != 0){ //if new information received, then update leds one by one
      for(int rowIndex = 0; rowIndex < NUM_ROWS; rowIndex++){ 
        for(int colIndex = 0; colIndex < NUM_COLS; colIndex++){
          int stripIndex = (rowIndex%2==0 ? (rowIndex * NUM_COLS) + colIndex : ((rowIndex + 1) * NUM_COLS) - (colIndex + 1)); //alternate rows are reversed in direction, this calculates their strip position
          if((NUM_ROWS - (rowIndex + 1)) < int(serialBytes[colIndex])){ //light rows up to current readout, (inverting rowIndex : upside down in my case)
            strip.setLEDcolorPWM(stripIndex, red, green, blue);            
          }
          else{
            strip.setLEDcolorPWM(stripIndex, 0, 0, 0);            
          }
        }
      }
    }
  }  
}

/** These values worked for my strip. They can be change if the display is unstable 
(see example sketches in the adafruit HL1606stripPWM.h library for how). */
void configureStrip(){
  strip.setPWMbits(3);
  strip.setSPIdivider(32);
  strip.setCPUmax(70);
  strip.begin();
}
