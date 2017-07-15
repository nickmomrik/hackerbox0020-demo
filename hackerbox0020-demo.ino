// Demo which combines several of the individual demos from
// http://www.instructables.com/id/HackerBoxes-0020-Summer-Camp/

#include <Adafruit_NeoPixel.h>
#include "WiFi.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// Pins
#define buzzerPin 18
#define touch1Pin 12
#define touch2Pin 13
#define touch3Pin 14
#define touch4Pin 27
#define touch5Pin 15
#define ledPIN     5
#define TFT_CS    19
#define TFT_DC    22
#define TFT_MOSI  23
#define TFT_CLK   26
#define TFT_RST   21
#define TFT_MISO  25

int touchThreshold = 40;
int pixelCount = 5;
int lux = 20;

// Touch states
bool touch1 = false;
bool touch2 = false;
bool touch3 = false;
bool touch4 = false;
bool touch5 = false;

// Buzzer tones
const int f = 349;
const int gS = 415;
const int a = 440;
const int cH = 523;
const int eH = 659;
const int fH = 698;
const int e6 = 1319;
const int g6 = 1568;
const int a6 = 1760;
const int as6 = 1865;
const int b6 = 1976;
const int c7 = 2093;
const int d7 = 2349;
const int e7 = 2637;
const int f7 = 2794;
const int g7 = 3136;

Adafruit_NeoPixel strip = Adafruit_NeoPixel( pixelCount, ledPIN, NEO_GRB + NEO_KHZ800 );

char ssid[] = "nickmomrik";  //put your handle after the underscore
Adafruit_ILI9341 tft = Adafruit_ILI9341( TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO );

void setup() {
  pinMode( buzzerPin, OUTPUT );

  touchAttachInterrupt( touch1Pin, touchLeft, touchThreshold );
  touchAttachInterrupt( touch2Pin, touchRight, touchThreshold );
  touchAttachInterrupt( touch3Pin, touchUp, touchThreshold );
  touchAttachInterrupt( touch4Pin, touchDown, touchThreshold );
  touchAttachInterrupt( touch5Pin, touchLoner, touchThreshold );

  strip.begin();
  delay( 1000 );

  tft.begin();
  tft.setRotation( 3 ); // rotate 3 * ( pi / 2 )

  clearPixels();
}

void loop() {
  if ( touch1 ) {
    MarioTheme();
    touch1 = false;
  }

  if ( touch2 ) {
    ImperialMarch();
    touch2 = false;
  }

  if ( touch3 ) {
    rainbowCycle( 5 );
    touch3 = false;
  }

  if ( touch4 ) {
    theaterChaseRainbow( 5 );
    touch4 = false;
  }

  if ( touch5 ) {
    wifiScan2LCD();
    // start broadcating SSID (AP on)
    WiFi.softAP( ssid, NULL, 1, 0, 1 );
    delay( 5000 );
    WiFi.softAPdisconnect( 1 );

    touch5 = false;
  }
}

void clearPixels() {
  for ( int i = 0; i < strip.numPixels(); i++ ) {
    strip.setPixelColor( i, strip.Color( 0, 0, 0 ) );
  }
  strip.show();
}

void touchLeft() {
  touch1 = true;
}

void touchRight() {
  touch2 = true;
}

void touchUp() {
  touch3 = true;
}

void touchDown() {
  touch4 = true;
}

void touchLoner() {
  touch5 = true;
}

void beep( int tone, int duration ) {
  for ( long i = 0; i < duration * 900L; i += tone * 1 ) {
    digitalWrite( buzzerPin, HIGH );
    delayMicroseconds( 0.5 * tone );
    digitalWrite( buzzerPin, LOW );
    delayMicroseconds( 0.5 * tone );
  }

  delay( 30 );
}

void ImperialMarch() {
  beep( a, 500 );
  beep( a, 500 );
  beep( a, 500 );
  beep( f, 350 );
  beep( cH, 150 );
  beep( a, 500 );
  beep( f, 350 );
  beep( cH, 150 );
  beep( a, 650 );
  delay( 500 );
  beep( eH, 500 );
  beep( eH, 500 );
  beep( eH, 500 );
  beep( fH, 350 );
  beep( cH, 150 );
  beep( gS, 500 );
  beep( f, 350 );
  beep( cH, 150 );
  beep( a, 650 );
}

void  MarioTheme() {
  beep( e7, 150 );
  beep( e7, 150 );
  delay( 150 );
  beep( e7, 150 );
  delay( 150 );
  beep( c7, 150 );
  beep( e7, 150 );
  delay( 150 );
  beep( g7, 150 );
  delay( 450 );
  beep( g6, 150 );
  delay( 450 );
  beep( c7, 150 );
  delay( 300 );
  beep( g6, 150 );
  delay( 300 );
  beep( e6, 150 );
  delay( 300 );
  beep( a6, 150 );
  delay( 150 );
  beep( b6, 150 );
  delay( 150 );
  beep( as6, 150 );
  beep( a6, 150 );
  delay( 150 );
  beep( g6, 112 );
  beep( e7, 112 );
  beep( g7, 112 );
  beep( a6, 150 );
  delay( 150 );
  beep( f7, 150 );
  beep( g7, 150 );
  delay( 150 );
  beep( e7, 150 );
  delay( 150 );
  beep( c7, 150 );
  beep( d7, 150 );
  beep( b6, 150 );
}

// Pixel functions taken from From the NeoPixel strandtest example
void rainbowCycle( uint8_t wait ) {
  uint16_t i, j;

  for ( j = 0; j < 256 * 5; j++ ) { // 5 cycles of all colors on wheel
    for ( i = 0; i < strip.numPixels(); i++ ) {
      strip.setPixelColor( i, Wheel( ( ( i * 256 / strip.numPixels() ) + j ) & 255 ) );
    }
    strip.show();
    delay( wait );
  }

  clearPixels();
}

// Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow( uint8_t wait ) {
  for ( int j = 0; j < 256; j++ ) {     // cycle all 256 colors in the wheel
    for ( int q = 0; q < 3; q++ ) {
      for ( uint16_t i = 0; i < strip.numPixels(); i = i + 3 ) {
        strip.setPixelColor( i + q, Wheel( ( i + j ) % 255 ) );    //turn every third pixel on
      }
      strip.show();

      delay( wait );

      for ( uint16_t i = 0; i < strip.numPixels(); i = i + 3 ) {
        strip.setPixelColor( i + q, 0 );        //turn every third pixel off
      }
    }
  }

  clearPixels();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel( byte WheelPos ) {
  WheelPos = 255 - WheelPos;
  if ( WheelPos < 85 ) {
    return strip.Color( 255 - WheelPos * 3, 0, WheelPos * 3 );
  }
  if ( WheelPos < 170 ) {
    WheelPos -= 85;
    return strip.Color( 0, WheelPos * 3, 255 - WheelPos * 3 );
  }
  WheelPos -= 170;

  return strip.Color( WheelPos * 3, 255 - WheelPos * 3, 0 );
}

void wifiScan2LCD() {
  int netsfound;
  int displaylines = 13;
  
  tft.fillScreen( ILI9341_BLACK );
  tft.setCursor( 0, 0 );
  tft.setTextColor( ILI9341_YELLOW );
  tft.setTextSize( 3 );
  tft.println( " HackerBox #0020" );
  tft.println( " WiFi Networks" );
  tft.setTextColor( ILI9341_GREEN );
  tft.setTextSize( 2 );
  tft.println( " " );

  netsfound = WiFi.scanNetworks();
  if ( netsfound == 0 ) {
    tft.println( ". . ." );
    displaylines--;
  }

  for ( int i = 0; i < netsfound; ++i ) {
    // Print SSID and RSSI for each network found
    tft.print( " [" );
    tft.print( WiFi.RSSI( i ) );
    tft.print( "] " );
    tft.print( WiFi.SSID( i ).substring( 0, 17 ) );
    tft.println( ( WiFi.encryptionType( i ) == WIFI_AUTH_OPEN ) ? " " : "*" );
    delay( 50 );
    displaylines--;
  }
}

