#include "HL1606stripPWM.h"

//Define a bunch of information about the layout and the wiring
//The circuit relies on using the Arduino's SPI Pins 11 and 13 for DATA (DI) and CLOCK (CI)
//when sending information along the chain and Pin 10 for the LATCH (LI)
#define NUM_COLS 8
#define NUM_ROWS 8
#define NUM_UNITS 64
#define LATCH_PIN 10

//creates an object to control a strip
HL1606stripPWM strip = HL1606stripPWM(NUM_UNITS, LATCH_PIN);
//used to store bytes received from the laptop over the serial connection
char serialBytes[NUM_COLS];

//these numbers are used to control the pixel color (all pixels are the same in this simple sketch)
int red=255,green=255,blue=255;

void setup() {
  Serial.begin(9600);
  Serial.println("Started");
  configureStrip();
}

void loop() {
  
  //causes the red green and blue components to vary slowly in random directions
  int walk = 50;
  red += random(walk) - random(walk);
  green += random(walk) - random(walk);
  blue += random(walk) - random(walk);
  red = constrain(red,0,255);
  green = constrain(green,0,255);
  blue = constrain(blue,0,255);
  
  //checks if there's any bytes to read from the serial
  if(Serial.available() > 0){
    //tries to read in a whole line
    int bytesRead = Serial.readBytesUntil('\n', serialBytes, NUM_COLS);
  //if new information received, then update leds one by one
    if(bytesRead != 0){ 
      for(int rowIndex = 0; rowIndex < NUM_ROWS; rowIndex++){ 
        for(int colIndex = 0; colIndex < NUM_COLS; colIndex++){
          int stripIndex = (rowIndex%2==0 ? (rowIndex * NUM_COLS) + colIndex : ((rowIndex + 1) * NUM_COLS) - (colIndex + 1)); //alternate rows are reversed in direction, this calculates their strip position
          //check if this light should be lit (it is below the bar height sent by the laptop)
          //rowIndex had to be interpreted as upside down in my case because the strip ran from bottom right
          if((NUM_ROWS - (rowIndex + 1)) < int(serialBytes[colIndex])){ 
            //turn on the LED
            strip.setLEDcolorPWM(stripIndex, red, green, blue);            
          }
          else{
            //turn off the LED
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
