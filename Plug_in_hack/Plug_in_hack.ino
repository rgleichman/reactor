#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>

#define PIN 6
#define num_bulbs 24

//typedef struct rgb rgb;
struct rgb{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

typedef struct rgb Rgb;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(num_bulbs, PIN, NEO_GRB + NEO_KHZ800);
CapacitiveSensor   cs_4_8 = CapacitiveSensor(4,8);        // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil
uint16_t j;
int mode = 0;
int prev_touch = 0;
int num_modes = 2;
//bright_mult is from 0 to 255
int bright_mult = 0;
uint32_t cyan = strip.Color(0, 255, 255);
uint32_t yellow = strip.Color(255, 150, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t green = strip.Color(0, 255, 0);
uint32_t red = strip.Color(255, 0, 0);
uint32_t no_color = strip.Color(0, 0, 0);
uint32_t white = strip.Color(255, 255, 255);
uint32_t mode_9_count = 0;
uint8_t but_count;
boolean but_done = false;
//array with the indecies of the lights that will be yellow. Each row is a USB group.
#define yellow_bulb_rows 3
#define yellow_bulb_cols 5
int yellow_bulbs[yellow_bulb_rows][yellow_bulb_cols] = 
{
  {
    22, 23, 0, 1, 2                                              }
  ,
  {
    6, 7, 8, 9, 10                                            }
  ,
  {
    14, 15, 16, 17, 18                                            }
};
int bulb_row = 0;
//bat_indicator 0 = empty, 239 = full
#define max_bat 239
//bat_indicator 0 = empty, 239 = full
uint8_t bat_indicator = 0;
//bat_charge 0 = empty, 239 = full
uint8_t bat_charge = 119;
int loop_num = 0;
//for mode 7
int16_t half_light_counter = 0;
uint8_t moving_bright = 0;
uint8_t moving_white = 0;

void setup() {
  cs_4_8.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  j=0;
}

void loop() {
  long start = millis();
  //long total3 =  cs_4_8.capacitiveSensor(3);
  //int touch_thresh = 50;
  //int plug_in_thresh = 300;

  //Serial.print(millis() - start);        // check on performance in milliseconds
  //Serial.print("\t");                    // tab character for debug windown spacing

  //Serial.println(total3);                // print sensor output 3

    delay(5);                             // arbitrary delay to limit data to serial port 
  //int touch = total3 > touch_thresh;
  //if(total3 > plug_in_thresh){
    mode = 1;
  //}
  //else{
  //  mode = 0;

  //}
  strip.setBrightness(255);
  setOneColor(no_color);
  if(mode == 0){
    moving_bright = 0;
    half_light_counter = 0;
    uint8_t bright_increase = 20;
    for(uint32_t bulb = 0; bulb < 5; bulb++){
      //last bulb, so decrease intensity
      uint8_t decreased_white = 249 - moving_white;
      if(bulb == 0){
        strip.setPixelColor((bulb + mode_9_count) % num_bulbs,decreased_white, decreased_white, decreased_white);
      }
      else if(bulb == 4){
        // first bulb, so increase intensity
        strip.setPixelColor((bulb + mode_9_count) % num_bulbs, moving_white, moving_white, moving_white);
      }
      else{
        strip.setPixelColor((bulb + mode_9_count) % num_bulbs, white);

      }
    }
    if(moving_white >= 250 - bright_increase){
      mode_9_count = (mode_9_count + 1) % num_bulbs;
    }
    moving_white = (moving_white + bright_increase) % 250;
  }
  if(mode == 1){
    if(but_done){
      but_done = false;
      but_count = (but_count + 1) % 3;
    }
    switch(but_count){
    case 0:
      multi_butter(8, 0, 0, 255);
      break;
    case 1:
      multi_butter(0, 0, 255, 0);
      break;
    case 2:
      multi_butter(16, 0, 0, 255);
      break;
    }
  }
  strip.show();
  //prev_touch = touch;
  loop_num= loop_num + 1 %1000;
}

void butterfly(uint8_t red, uint8_t green, uint8_t blue){
  uint8_t bright_increase = 15;
  uint8_t scale_red = moving_bright*red/255;
  uint8_t scale_green = moving_bright*green/255;
  uint8_t scale_blue = moving_bright*blue/255;
  if(half_light_counter > 0){
    strip.setPixelColor((half_light_counter - 1 + num_bulbs / 2 ) % num_bulbs, red - scale_red, 
    green - scale_green, blue- scale_blue);

    strip.setPixelColor((24 - (half_light_counter - 1) + num_bulbs / 2) % num_bulbs, red - scale_red, 
    green - scale_green, blue- scale_blue);
  }
  if(half_light_counter < num_bulbs / 2 + 1){
    strip.setPixelColor(((half_light_counter - 1) + num_bulbs / 2 + 1)%num_bulbs, scale_red, scale_green, scale_blue);
    strip.setPixelColor((24 - (half_light_counter - 1) + num_bulbs / 2 - 1) % num_bulbs, scale_red, scale_green, scale_blue);
  }
  if(moving_bright >= 250 - bright_increase){
    half_light_counter = (half_light_counter + 1) % (num_bulbs / 2 + 2);
  }
  moving_bright = (moving_bright + bright_increase) % 250;
}
void multi_butter(uint8_t start, uint8_t red, uint8_t green, uint8_t blue){
  uint8_t bright_increase = 30;
  uint8_t scale_red = moving_bright*red/255;
  uint8_t scale_green = moving_bright*green/255;
  uint8_t scale_blue = moving_bright*blue/255;
  uint8_t offset = half_light_counter;
  if(half_light_counter > 0){
    strip.setPixelColor((offset - 1 + num_bulbs / 2 + start) % num_bulbs, red - scale_red, 
    green - scale_green, blue- scale_blue);

    strip.setPixelColor((24 - (offset - 1) + num_bulbs / 2 + start) % num_bulbs, red - scale_red, 
    green - scale_green, blue- scale_blue);
  }
  if(half_light_counter < num_bulbs / 2 + 1){
    strip.setPixelColor(((offset - 1) + num_bulbs / 2 + 1 + start) % num_bulbs, scale_red, scale_green, scale_blue);
    strip.setPixelColor((24 - (offset - 1) + num_bulbs / 2 - 1 + start) % num_bulbs, scale_red, scale_green, scale_blue);
  }
  if(moving_bright >= 250 - bright_increase){
    half_light_counter = (half_light_counter + 1) % (num_bulbs / 2 + 2);
    if(half_light_counter == 0){
      but_done = true;
    }
  }
  moving_bright = (moving_bright + bright_increase) % 250;
}
void all_butter(uint8_t start, uint8_t red, uint8_t green, uint8_t blue){
  uint8_t bright_increase = 15;
  uint8_t scale_red = moving_bright*red/255;
  uint8_t scale_green = moving_bright*green/255;

  uint8_t scale_blue = moving_bright*blue/255;
  uint8_t offset = half_light_counter;
  if(half_light_counter > 0){
    addToColor(&strip, (offset - 1 + num_bulbs / 2 + start) % num_bulbs, red - scale_red, 
    green - scale_green, blue- scale_blue);

    addToColor(&strip, (24 - (offset - 1) + num_bulbs / 2 + start) % num_bulbs, red - scale_red, 
    green - scale_green, blue- scale_blue);
  }
  if(half_light_counter < num_bulbs / 2 + 1){
    addToColor(&strip, ((offset - 1) + num_bulbs / 2 + 1 + start) % num_bulbs, scale_red, scale_green, scale_blue);
    addToColor(&strip, (24 - (offset - 1) + num_bulbs / 2 - 1 + start) % num_bulbs, scale_red, scale_green, scale_blue);
  }
  if(moving_bright >= 250 - bright_increase){
    half_light_counter = (half_light_counter + 1) % (num_bulbs / 2 + 2);
    if(half_light_counter == 0){
      but_done = true;
    }
  }
  moving_bright = (moving_bright + bright_increase) % 250;
}

void setOneColor(uint32_t c){
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
}

//Gets the individual colors as an array
//red = [2]
//green = [1]
//blue = [0]

void getColors(uint32_t color, struct rgb *rgb_return){
  uint8_t * colors = (uint8_t *) & color;
  rgb_return-> red = colors[2];
  rgb_return -> green = colors[1];
  rgb_return -> blue = colors[0];
}

void addToColor(Adafruit_NeoPixel* strip, uint8_t n, uint8_t red, uint8_t green, uint8_t blue){
  Rgb old_colors;
  getColors(strip -> getPixelColor(0), & old_colors);
  strip -> setPixelColor(n, min(255, old_colors.red + red), min(255, old_colors.green + green), min(255, old_colors.blue + blue));
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



























