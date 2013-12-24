// Low power NeoPixel goggles example.  Makes a nice blinky display
// with just a few LEDs on at any time...uses MUCH less juice than
// rainbow display!

#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 0
#define BUTTON_PIN 1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, NEOPIXEL_PIN);

uint8_t  mode   = 2, // Current animation effect
// "left" is closes to cpu
         leftOff = 7, // Position of spinny eyes
         rightOff = 2,
         pos = 8;
uint8_t  i; // generic index

uint8_t seen_button_up = 1; //1:button has been up, 0 waiting for up
uint8_t brightness_mode = 0; //0-5 levels of brightness. 0 = pulse (sleep) mode

uint8_t testpos = 0;
uint32_t color  = 0xFF0000; // Start red
uint32_t prevTime;

int32_t hires_pos = 0, // 100x actual pos so we can fake floats
  inertia = 0,
  moment =0,
  spring_force = 0;
 
  
#define system_size 8*256
#define scale2pixel 256
#define friction  230 // 90% of 256 = 10% drag
#define spring_constant 92 // 36% of 256
#define denom 256 // binary fraction time!
const uint8_t vFlip[] PROGMEM ={
  0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 
  0x0, 0xF, 0xE, 0xD, 0xC, 0xB, 0xA, 0x9
};

const uint8_t brightnessValues[] PROGMEM ={
  0x00, 0x07, 0x20, 0x40, 0x80, 0xFF 
};

void setup() {
  pinMode(BUTTON_PIN, INPUT); // make this an input
  digitalWrite(BUTTON_PIN, HIGH); // ...with a pullup resistor
  randomSeed(analogRead(0)); // Seed the random number generator with some noise off pin 0
  pixels.begin();
  pixels.setBrightness(5); // 1/3 brightness (85)
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
    BackgroundDelay(10);
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
    BackgroundDelay(50);
   
    break;
    
    
    
    case 2: 
   // googly
   // ======================================================

    inertia = ((inertia -  (hires_pos /3 )) * friction) /denom;
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
 
    BackgroundDelay(24);
    
    // randomly add an impulse:
    if (random(60) == 1){
     inertia = inertia + random(800);

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
    BackgroundDelay(60);
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
  BackgroundDelay(100);
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
 
void BackgroundDelay(unsigned long delay_milliseconds){
  unsigned long now = millis();
  while ((now + delay_milliseconds) > millis()){
    
    if (digitalRead(BUTTON_PIN) == LOW) {
      if (seen_button_up){
        brightness_mode = (brightness_mode+1)%5;
        seen_button_up = 0; // mark that we've seen this button press
       // pixels.setBrightness(32*brightness_mode);
       pixels.setBrightness(pgm_read_byte_near(brightnessValues+brightness_mode));
       
      }
    } else {
      seen_button_up = 1;
    }
         
  } // timing while
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
