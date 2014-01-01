#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, PIN, NEO_GRB + NEO_KHZ800);
CapacitiveSensor   cs_4_8 = CapacitiveSensor(4,8);        // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil
uint16_t j;
int mode = 4;
int prev_touch = 0;
int num_modes = 5;
//bright_mult is from 0 to 255
int bright_mult = 0;
uint32_t cyan = strip.Color(0, 255, 255);
uint32_t yellow = strip.Color(255, 150, 0);
uint32_t no_color = strip.Color(0, 0, 0);
//array with the indecies of the lights that will be yellow. Each row is a USB group.
#define yellow_bulb_rows 3
#define yellow_bulb_cols 5
int yellow_bulbs[yellow_bulb_rows][yellow_bulb_cols] = 
{
  {
    22, 23, 0, 1, 2    }
  ,
  {
    6, 7, 8, 9, 10  }
  ,
  {
    14, 15, 16, 17, 18  }
};
int bulb_row = 0;

void setup() {
  cs_4_8.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  j=0;
}

void loop() {
  long start = millis();
  long total3 =  cs_4_8.capacitiveSensor(30);
  int touch_thresh = 500;

  Serial.print(millis() - start);        // check on performance in milliseconds
  Serial.print("\t");                    // tab character for debug windown spacing

  Serial.println(total3);                // print sensor output 3

    delay(5);                             // arbitrary delay to limit data to serial port 
  int touch = total3 > touch_thresh;
  if(touch && !prev_touch){
    mode = (mode + 1) % num_modes;
  }
  if(mode == 0){    
    rainbowCycle(10);
    j = (j+1) % 256;
  }
  if(mode == 1){
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }
  if(mode == 2){
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 255, 255, 255);
    }
  }
  if(mode == 3){
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }
  if(mode == 4){
    //    for(int i=0; i<strip.numPixels(); i++) {
    //      strip.setPixelColor(i, yellow);
    //    }
    setOneColor(no_color);
    for(int col = 0; col < yellow_bulb_cols; col++){
      strip.setPixelColor(yellow_bulbs[bulb_row][col], yellow);
    }
    strip.setBrightness(abs(bright_mult-100)+20);
    bright_mult = (bright_mult + 1) % 200;
    if (bright_mult == 0){
      bulb_row = (bulb_row + 1) % 3;
    }
  }
  strip.show();
  prev_touch = touch;
}

void setOneColor(uint32_t c){
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i;

  //  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
  for(i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
  }
  strip.show();
  delay(wait);
  // }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}






