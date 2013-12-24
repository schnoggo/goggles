// Low power NeoPixel goggles example.  Makes a nice blinky display
// with just a few LEDs on at any time...uses MUCH less juice than
// rainbow display!

#include <Adafruit_NeoPixel.h>

#define PIN 0

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, PIN);

uint8_t  mode   = 2, // Current animation effect
// "left" is closes to cpu
         leftOff = 7, // Position of spinny eyes
         rightOff = 2,
         pos = 8;
uint8_t  i; // generic index

uint8_t testpos = 0;
uint32_t color  = 0xFF0000; // Start red
uint32_t prevTime;

int32_t hires_pos = 0, // 100x actual pos so we can fake floats
  inertia = 0,
  moment =0,
  spring_force = 0;
 
  
#define system_size 800
#define scale2pixel 100 // scale to 16 pixels (800*2/16)
#define friction  90 // (100-89)/100
#define spring_constant 36 // 36/100 = .36
const byte vFlip[] PROGMEM ={
  0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 
  0x0, 0xF, 0xE, 0xD, 0xC, 0xB, 0xA, 0x9
};

void setup() {
  randomSeed(analogRead(0));
  pixels.begin();
  pixels.setBrightness(128); // 1/3 brightness (85)
  prevTime = millis();
}

void loop() {
  
  uint32_t t;
  
  switch(mode) {

   case 0: // Random sparks - just one LED on at a time!
   // ======================================================
    i = random(32);
   // pixels.setPixelColor(i, color);
   pixels.setPixelColor(i, pixels.Color(SteppedColor(), SteppedColor(), SteppedColor() ));
    pixels.show();
    delay(10);
    pixels.setPixelColor(i, 0);
    
    break;
 
 
 
   case 1: // Spinny wheels (8 LEDs on at a time)
  // ======================================================
    for(i=0; i<16; i++) {
      uint32_t c = 0; // turn off non-selected pixels
      if(((pos + i) & 7) < 2) c = color; // 4 pixels on...
      pixels.setPixelColor(  NormalizeRingPos(i+leftOff), c); // First eye
      pixels.setPixelColor(16 + NormalizeRingPos(16-i+rightOff)  , c); // Second eye (flipped)
    }
    pixels.show();
    pos = pos++ % 16;
    delay(50);
   
    break;
    
    
    
    case 2: 
   // googly
   // ======================================================
 
    // inertia = (inertia -  ((hires_pos /3 ) * friction))  /100;
    inertia = ((inertia -  (hires_pos /3 ))*90)/100;
   // if (moment <25) { moment=0;} // stop when close to zero;
   // inertia = moment;
    hires_pos = hires_pos + inertia;
    if (hires_pos >= system_size){
      // FlashRing();
      hires_pos = -(hires_pos - system_size);
      inertia = -inertia;
    } else if (hires_pos < (-system_size)) {
     //  FlashRing();
      hires_pos = system_size + hires_pos;
      inertia = -inertia;
    }
    // + 8  to rotate 0 to bottom
   pos = NormalizeRingPos(8+ (hires_pos / scale2pixel)); 


    for(i=0; i<16; i++) {
      uint32_t c = 0;
     // if(pos == i) c = color; 
      if(RingDistance(pos, i)<2) c = color; 
      pixels.setPixelColor(    NormalizeRingPos(i+leftOff )  , c); // First eye
      pixels.setPixelColor( 16 +NormalizeRingPos(i+rightOff) , c); // Second eye (not flipped)
    }
    pixels.show();
 
    delay(24);
    
    // randomly add an impulse:
    if (random(60) == 1){
      // FlashRing();
     inertia = inertia + random(600)-30;
     // inertia = 300;
     //  NextColor();
    }
    
    
    break;
   
      case 3: // sequencer
  // ====================================================== 
      for(i=0; i<16; i++) {
      uint32_t c = 0; // turn off non-selected pixels
      if(testpos == i) {c= 0xFFFF00;} // 4 pixels on...
      pixels.setPixelColor(  NormalizeRingPos(i+leftOff), c); // First eye
      pixels.setPixelColor(16 + NormalizeRingPos(i+rightOff)  , c); // Second eye (flipped)
    }
    testpos++;
    if (testpos>15){testpos=0;}
    delay(60);
    pixels.show();
  }

  t = millis();
  if((t - prevTime) > 8000) {      // Every 8 seconds... change modes
    mode++;                        // Next mode
    if(mode > 3) {                 // End of modes?
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

uint8_t NormalizeRingPos(uint8_t realPos){
  
  while (realPos < 0) { realPos += 16;}
  while (realPos > 15) { realPos -= 16; }  
  return realPos;
}

uint8_t ReflectVertical(uint8_t ringPos){
}

uint8_t RingDistance(int8_t pos1, int8_t pos2){
  // given two points on the NeoPixel Ring, return
  // unsigned distance between them.
  int8_t retVal= abs(pos1-pos2);
  if( retVal>8){retVal = 16-retVal;}
  return retVal;
}
 uint8_t SteppedColor(){
   // return a non-continuous value for a color axis
   return random(4)*64;
 }
/*
Left-to-right sweep:
0: C C
1: D B
2: E A
3: F 9
4: 0 8
5: 1 7
6: 2 6
7: 3 5
8: 4 4

Reflect:
0: 8
1: 7
2: 6
3: 5
4: 4
5: 3
6: 2
7: 1
8: 0
9: F
A: E
B: D
C: C
D: B
E: A
F: 9
*/
