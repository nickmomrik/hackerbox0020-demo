/* 
  Demo which combines several of the individual demos from
  http://www.instructables.com/id/HackerBoxes-0020-Summer-Camp/

  SD Card code was a big help from https://github.com/lmarty/Hackerboxes20

  http://www.arduino.cc/en/Tutorial/TFTBitmapLogo
 */

#include "WiFi.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_NeoPixel.h"
#include <mySD.h>

// Pins
#define PIEZO_PIN     18
#define NEOPIXEL_PIN   5

#define TOUCH1_PIN    12
#define TOUCH2_PIN    13
#define TOUCH3_PIN    14
#define TOUCH4_PIN    27
#define TOUCH5_PIN    15

#define TFT_CLK_PIN   26
#define TFT_CS_PIN    19
#define TFT_DC_PIN    22
#define TFT_MISO_PIN  25
#define TFT_MOSI_PIN  23
#define TFT_RST_PIN   21

#define SD_CS_PIN     17
#define SD_MISO_PIN    4
#define SD_MOSI_PIN   16
#define SD_SCK_PIN     0

int pixelCount = 5;
int touchThreshold = 40;
char logoFile[] = "hackerboxes-logo.bmp";

// SD Card
Sd2Card card;
SdVolume volume;
SdFile root;

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

Adafruit_NeoPixel strip = Adafruit_NeoPixel( pixelCount, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800 );

Adafruit_ILI9341 tft = Adafruit_ILI9341( TFT_CS_PIN, TFT_DC_PIN, TFT_MOSI_PIN, TFT_CLK_PIN, TFT_RST_PIN, TFT_MISO_PIN );

void setup() {
  pinMode( PIEZO_PIN, OUTPUT );

  touchAttachInterrupt( TOUCH1_PIN, touchLeft, touchThreshold );
  touchAttachInterrupt( TOUCH2_PIN, touchRight, touchThreshold );
  touchAttachInterrupt( TOUCH3_PIN, touchUp, touchThreshold );
  touchAttachInterrupt( TOUCH4_PIN, touchDown, touchThreshold );
  touchAttachInterrupt( TOUCH5_PIN, touchLoner, touchThreshold );

  tft.begin();
  tft.setRotation( 3 ); // rotate 3 * ( pi / 2 )

  strip.begin();
  strip.setBrightness( 128 );
  strip.clear();
  strip.show();

  Serial.begin( 115200 );

  delay( 2000 );
  Serial.print( "Initializing SD card..." );
  if ( ! card.init( SPI_HALF_SPEED, SD_CS_PIN, SD_MOSI_PIN, SD_MISO_PIN, SD_SCK_PIN ) ) {
    Serial.println( " failed!" );
  } else {
    Serial.println( " success!" );
  }

  display_logo();
}

void loop() {
  if ( touch1 ) {
    // Read int from file, increment, and write it back for next read

    touch1 = false;
  }

  if ( touch2 ) {
    ImperialMarch();

    touch2 = false;
  }

  if ( touch3 ) {
    sd_info();

    touch3 = false;
  }

  if ( touch4 ) {
    rainbowCycle( 5 );

    touch4 = false;
  }

  if ( touch5 ) {
    wifiScan2LCD();

    touch5 = false;
  }
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
    digitalWrite( PIEZO_PIN, HIGH );
    delayMicroseconds( 0.5 * tone );
    digitalWrite( PIEZO_PIN, LOW );
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

  strip.clear();
  strip.show();
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
  tft.println( "" );

  netsfound = WiFi.scanNetworks();
  if ( netsfound == 0 ) {
    tft.println( ". . ." );
    displaylines--;
  }

  for ( int i = 0; i < netsfound; ++i ) {
    // Print SSID and RSSI for each network found
    tft.print( " " );
    tft.print( WiFi.RSSI( i ) );
    tft.print( " " );
    tft.print( WiFi.SSID( i ).substring( 0, 19 ) );
    tft.println( ( WiFi.encryptionType( i ) == WIFI_AUTH_OPEN ) ? "" : "*" );
    delay( 50 );
    displaylines--;
  }
}

void display_logo() {
  //logoFile;
}

void sd_info() {
  tft.fillScreen( ILI9341_BLACK );
  tft.setCursor( 0, 0 );
  tft.setTextColor( ILI9341_GREEN );
  tft.setTextSize( 2 );
  tft.print( " Card type: " );
  switch( card.type() ) {
    case SD_CARD_TYPE_SD1:
      tft.println( "SD1" );
      break;
    case SD_CARD_TYPE_SD2:
      tft.println( "SD2" );
      break;
    case SD_CARD_TYPE_SDHC:
      tft.println( "SDHC" );
      break;
    default:
      tft.println( "Unknown" );
  }

  if ( ! volume.init( card ) ) {
    tft.println( " " );
    tft.println( " Could not find" );
    tft.println( " FAT16/FAT32 partition." );
    tft.println( " Make sure you've" );
    tft.println( " formatted the card" );
    tft.println( " or reset the board." );

    return;
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  float volumesize_f;
  tft.print( " Volume type: FAT");
  tft.println( volume.fatType(), DEC );

  volumesize = volume.blocksPerCluster();   // clusters are collections of blocks
  volumesize *= volume.clusterCount();      // we'll have a lot of clusters
  volumesize *= 512;                        // SD card blocks are always 512 bytes
  volumesize_f = volumesize / 1024.0;       // Convert to bytes
  volumesize_f /= 1024;                     // Convert to Mbytes
  volumesize_f /= 1024;                     // Convert to Gbytes
  tft.print( " Volume size: " );
  tft.print( volumesize_f );
  tft.println( " GB" );
}

