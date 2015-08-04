#include <Adafruit_NeoPixel.h>
#define PIN 6
char usart_val=0;
char usart_table=0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Goodnight moon!");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
    if (Serial.available())
  {
      usart_val=Serial.read();
      if(usart_val == 'A') usart_table = 1;
      if(usart_val == 'B') usart_table = 2;
      usart_val = 0;     
  }

       if(usart_table == 1)
      {
          digitalWrite(4,HIGH);
          digitalWrite(5,LOW);
          if(usart_table == 1)
          colorWipe(strip.Color(255, 0, 0), 50); // Red
          if(usart_table == 1)
          colorWipe(strip.Color(0, 255, 0), 50); // Green
          if(usart_table == 1)
          colorWipe(strip.Color(0, 0, 255), 50); // Blue
          // Send a theater pixel chase in...
          if(usart_table == 1)
          theaterChase(strip.Color(127, 127, 127), 50); // White
          if(usart_table == 1)
          theaterChase(strip.Color(127,   0,   0), 50); // Red
          if(usart_table == 1)
          theaterChase(strip.Color(  0,   0, 127), 50); // Blue
          
          if(usart_table == 1)
          rainbow(20);
          if(usart_table == 1)
          rainbowCycle(20);
          if(usart_table == 1)
          theaterChaseRainbow(50);


      }
      if(usart_table == 2)
      {
          digitalWrite(4,HIGH);
          digitalWrite(5,HIGH);
        
          strip.begin();
          strip.show(); // Initialize all pixels to 'off'


      }
}

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
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
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
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

