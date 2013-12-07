// Low power NeoPixel goggles example.  Makes a nice blinky display
// with just a few LEDs on at any time...uses MUCH less juice than
// rainbow display!

#include <Adafruit_NeoPixel.h>

#define PIN 0

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, PIN);

uint8_t  mode   = 3, // Current animation effect
         leftOff = 7, // Position of spinny eyes
         rightOff = 2,
         pos = 8;
uint8_t  i; // generic index

uint32_t color  = 0xFF0000; // Start red
uint32_t prevTime;

void setup() {
  pixels.begin();
  pixels.setBrightness(85); // 1/3 brightness
  prevTime = millis();
}

void loop() {
  
  uint32_t t;
  
  switch(mode) {

   case 0: // Random sparks - just one LED on at a time!
    i = random(32);
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(10);
    pixels.setPixelColor(i, 0);
    break;
 
   case 1: // Spinny wheels (8 LEDs on at a time)

    for(i=0; i<16; i++) {
      uint32_t c = 0; // turn off non-selected pixels
      if(((pos + i) & 7) < 2) c = color; // 4 pixels on...
      pixels.setPixelColor(  (i+leftOff) % 16, c); // First eye
      pixels.setPixelColor(32 - ((i+rightOff) % 16 ), c); // Second eye (flipped)
    }
    pixels.show();
    pos = pos++ % 16;
    delay(50);
   
    break;
    
    case 3: 
   // googly
    for(i=0; i<16; i++) {
      uint32_t c = 0;
      if(pos == i) c = color; // 4 pixels on...
      pixels.setPixelColor(  (i+leftOff) % 16, c); // First eye
      pixels.setPixelColor(((i+rightOff) % 16 + 16), c); // Second eye (not flipped)
    }
    pixels.show();
    pos = pos++ % 16;
    delay(40);
    break;
  }

  t = millis();
  if((t - prevTime) > 4000) {      // Every 8 seconds... change modes
    mode++;                        // Next mode
    if(mode > 2) {                 // End of modes?
      mode = 0;                    // Start modes over
      color >>= 8;                 // Next color R->G->B
      if(!color) color = 0xFF0000; // Reset to red
    }
    ClearRings();
    prevTime = t;
  }
}

void ClearRings(){
    for(i=0; i<32; i++) pixels.setPixelColor(i, 0);
}
