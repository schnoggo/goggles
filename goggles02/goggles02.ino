// Low power NeoPixel goggles example.  Makes a nice blinky display
// with just a few LEDs on at any time...uses MUCH less juice than
// rainbow display!

#include <Adafruit_NeoPixel.h>

#define PIN 0

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, PIN);

uint8_t  mode   = 2, // Current animation effect
         leftOff = 7, // Position of spinny eyes
         rightOff = 2,
         pos = 8;
uint8_t  i; // generic index

uint32_t color  = 0xFF0000; // Start red
uint32_t prevTime;

int32_t hires_pos = 0, // 100x actual pos so we can fake floats
  inertia = 0,
  moment,
  spring_force = 0;
 
  
#define system_size 800
#define scale2pixel 100 // scale to 16 pixels (800*2/16)
#define friction  90 // (100-89)/100
#define spring_constant 36 // 36/100 = .36

void setup() {
  randomSeed(analogRead(0));
  pixels.begin();
  pixels.setBrightness(10); // 1/3 brightness (85)
  prevTime = millis();
}

void loop() {
  
  uint32_t t;
  
  switch(mode) {

   case 0: // Random sparks - just one LED on at a time!
   // ======================================================
    i = random(32);
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(10);
    pixels.setPixelColor(i, 0);
    break;
 
 
 
   case 1: // Spinny wheels (8 LEDs on at a time)
  // ======================================================
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
    
    
    
    case 2: 
   // googly
   // ======================================================
 
    inertia = (inertia -  ((hires_pos /3 ) * friction))  /100;
   // if (moment <25) { moment=0;} // stop when close to zero;
   // inertia = moment;
    hires_pos = hires_pos + inertia;
    if (hires_pos >= system_size){
       FlashRing();
      hires_pos = -(hires_pos - system_size);
      inertia = -inertia;
    } else if (hires_pos < (-system_size)) {
       FlashRing();
      hires_pos = system_size + hires_pos;
      inertia = -inertia;
    }
   pos = hires_pos / scale2pixel;
pos = (pos + 8) % 16;

    for(i=0; i<16; i++) {
      uint32_t c = 0;
      if(pos == i) c = color; // 4 pixels on...
      pixels.setPixelColor(    (i+leftOff ) % 16 , c); // First eye
      pixels.setPixelColor( 16 +(((16-i)+rightOff) % 16) , c); // Second eye (not flipped)
    }
    pixels.show();
 
    delay(30);
    
    // randomly add an impulse:
    if (random(40) == 1){
      // FlashRing();
     inertia = inertia + random(600)-30;
     // inertia = 300;
     //  NextColor();
    }
    
    
    break;
    
    
  }

  t = millis();
  if((t - prevTime) > 80000) {      // Every 8 seconds... change modes
    mode++;                        // Next mode
    if(mode > 2) {                 // End of modes?
      mode = 0;                    // Start modes over
    NextColor();
    }
    ClearRings();
    prevTime = t;
  }
}

void ClearRings(){
    SolidRing(0);
}

void SolidRing(uint32_t c){
    for(i=0; i<32; i++) pixels.setPixelColor(i, c);
    pixels.show();
}

void FlashRing(){
  SolidRing(0x222222);
  delay(100);
  SolidRing(0);
}

void NextColor(){
        color >>= 8;                 // Next color R->G->B
      if(!color) color = 0xFF0000; // Reset to red
}
