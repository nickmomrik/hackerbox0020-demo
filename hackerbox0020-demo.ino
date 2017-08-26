/* 
  Demo which builds on several of the individual demos from
  http://www.instructables.com/id/HackerBoxes-0020-Summer-Camp/

  SD Card code from
  https://github.com/nhatuan84/esp32-micro-sdcard

  bmpDraw from
  https://github.com/adafruit/Adafruit_ILI9341/
 */

#include "WiFi.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_NeoPixel.h"
#include "mySD.h"

// For reading BMPs
#define BUFFPIXEL 20

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
// Filenames must be 8 characters or less!
char logoFilename[] = "logo.bmp";
char rwFilename[] = "example.txt";

// SD Card
File root;
File example;
bool sd_ready = false;
String buffer;

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
  Serial.begin( 115200 );

  pinMode( PIEZO_PIN, OUTPUT );

  tft.begin();
  yield();
  tft.setRotation( 3 ); // rotate 3 * ( pi / 2 )

  strip.begin();
  strip.setBrightness( 128 );

  delay( 1000 );

  touchAttachInterrupt( TOUCH1_PIN, touchLeft, touchThreshold );
  touchAttachInterrupt( TOUCH2_PIN, touchRight, touchThreshold );
  touchAttachInterrupt( TOUCH3_PIN, touchUp, touchThreshold );
  touchAttachInterrupt( TOUCH4_PIN, touchDown, touchThreshold );
  touchAttachInterrupt( TOUCH5_PIN, touchLoner, touchThreshold );

  display_logo();

  black_out();
  tft.setCursor( 0, 30 );
  tft.setTextColor( ILI9341_BLUE );
  tft.setTextSize( 3 );
  tft.println( "      TOUCH" );
  tft.println( " " );
  tft.println( "        A" );
  tft.println( " " );
  tft.println( "       PAD" );
  tft.println( " " );
  tft.println( "       ..." );
}

void loop() {
  if ( touch1 ) {
    black_out();
    rw_sd_card();

    touch1 = false;
  }

  if ( touch2 ) {
    black_out();
    ImperialMarch();

    touch2 = false;
  }

  if ( touch3 ) {
    black_out();
    display_logo();

    touch3 = false;
  }

  if ( touch4 ) {
    black_out();
    rainbowCycle( 5 );

    touch4 = false;
  }

  if ( touch5 ) {
    black_out();
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
    tft.print( WiFi.RSSI( i ) );
    tft.print( " " );
    tft.print( WiFi.SSID( i ).substring( 0, 21 ) );
    tft.println( ( WiFi.encryptionType( i ) == WIFI_AUTH_OPEN ) ? "" : "*" );
    delay( 50 );
    displaylines--;
  }
}

void display_logo() {
  maybe_initialize_sd_card();

  bmpDraw( logoFilename, 0, 0 );

  delay( 3000 );
}

void black_out() {
  strip.clear();
  strip.show();

  tft.fillScreen( ILI9341_BLACK );
}

void maybe_initialize_sd_card() {
  if ( ! sd_ready ) {
    Serial.print( "Initializing SD card..." );
    if ( ! SD.begin( SD_CS_PIN, SD_MOSI_PIN, SD_MISO_PIN, SD_SCK_PIN ) ) {
      Serial.println( "failed!");
    } else {
      sd_ready = true;
      Serial.println( "success!" );

      /*
      root = SD.open( "/" );
      printDirectory( root, 0 );
      */
    }
  }
}

void printDirectory( File dir, int numTabs ) {
  // Begin at the start of the directory
  //dir.rewindDirectory();
  
  while ( true ) {
     File entry = dir.openNextFile();
     if ( ! entry ) {
       Serial.println( "** No more files **" );
       break;
     }
     for ( uint8_t i = 0; i < numTabs; i++ ) {
       Serial.print( '\t' );   // we'll have a nice indentation
     }
     // Print the 8.3 name
     Serial.print( entry.name() );
     // Recurse for directories, otherwise print the file size
     if ( entry.isDirectory() ) {
       Serial.println( "/" );
       printDirectory( entry, numTabs + 1 );
     } else {
       // files have sizes, directories do not
       Serial.print( "\t\t" );
       Serial.println( entry.size(), DEC );
     }
     entry.close();
   }
}

void rw_sd_card() {
  maybe_initialize_sd_card();

  if ( ! sd_ready ) {
    Serial.println( "SD Card not ready" );

    return;
  }

  if ( ! SD.exists( rwFilename ) ) {
    Serial.print( "Creating " );
    Serial.println( rwFilename );
    example = SD.open( rwFilename, FILE_WRITE );
    example.close();

    if ( ! SD.exists( rwFilename ) ) {
      Serial.print( "Error creating " );
      Serial.println( rwFilename );

      return;
    }
  }

  tft.setCursor( 0, 0 );
  tft.setTextColor( ILI9341_YELLOW );
  tft.setTextSize( 3 );
  
  example = SD.open( rwFilename );
  if ( example ) {
    int i = 0;
    while ( example.available() ) {
      buffer = example.readStringUntil( '\n' );
      tft.print( " " );
      tft.println( buffer );
      delay( 200 );

      i++;
      if ( 10 == i ) {
        black_out();
        tft.setCursor( 0, 0 );
        i = 0;
      }
    }

    example.close();

    example = SD.open( rwFilename, FILE_WRITE );
    if ( example ) {
      example.println( millis() );
      example.close();
    } else {
      Serial.print( "Error opening " );
      Serial.print( rwFilename );
      Serial.print( " for writing." );
    }
  } else {
    Serial.print( "Error opening " );
    Serial.print( rwFilename );
    Serial.print( " for reading." );
  }
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

void bmpDraw( char *filename, int16_t x, int16_t y ) {
  File     bmpFile;
  int      bmpWidth, bmpHeight;          // W+H in pixels
  uint8_t  bmpDepth;                     // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;               // Start of image data in file
  uint32_t rowSize;                      // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL];        // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof( sdbuffer ); // Current position in sdbuffer
  boolean  goodBmp = false;              // Set to true on valid header parse
  boolean  flip    = true;               // BMP is stored bottom-to-top
  int      w, h, row, col, x2, y2, bx1, by1;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if ( ! sd_ready ) {
    Serial.println( "SD Card not ready" );

    return;
  }

  if ( ( x >= tft.width() ) || ( y >= tft.height() ) ) {
    return;
  }

  Serial.println();
  Serial.print( F( "Loading image fram " ) );
  Serial.println( filename );

  // Open requested file on SD card
  bmpFile = SD.open( filename );
  if ( ! bmpFile ) {
    Serial.println( F( "File not found!" ) );

    return;
  }

  // Parse BMP header
  if ( read16( bmpFile ) == 0x4D42) { // BMP signature
    Serial.print( F( "File size: " ) );
    Serial.println( read32( bmpFile ) );
    (void) read32( bmpFile ); // Read & ignore creator bytes
    bmpImageoffset = read32( bmpFile ); // Start of image data
    Serial.print( F( "Image Offset: " ) );
    Serial.println( bmpImageoffset, DEC );
    // Read DIB header
    Serial.print( F( "Header size: " ) );
    Serial.println( read32( bmpFile ) );
    bmpWidth  = read32( bmpFile );
    bmpHeight = read32( bmpFile );
    if ( read16( bmpFile ) == 1 ) { // # planes -- must be '1'
      bmpDepth = read16( bmpFile ); // bits per pixel
      Serial.print( F( "Bit Depth: " ) );
      Serial.println( bmpDepth );
      if ( ( bmpDepth == 24 ) && ( read32( bmpFile ) == 0 ) ) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print( F( "Image size: " ) );
        Serial.print( bmpWidth );
        Serial.print( 'x' );
        Serial.println( bmpHeight );

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = ( bmpWidth * 3 + 3 ) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if ( bmpHeight < 0 ) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        x2 = x + bmpWidth  - 1; // Lower-right corner
        y2 = y + bmpHeight - 1;
        if ( ( x2 >= 0 ) && ( y2 >= 0 ) ) { // On screen?
          w = bmpWidth; // Width/height of section to load/display
          h = bmpHeight;
          bx1 = by1 = 0; // UL coordinate in BMP file
          if ( x < 0 ) { // Clip left
            bx1 = -x;
            x   = 0;
            w   = x2 + 1;
          }
          if ( y < 0 ) { // Clip top
            by1 = -y;
            y   = 0;
            h   = y2 + 1;
          }
          if ( x2 >= tft.width() ) {
            w = tft.width()  - x; // Clip right
          }
          if ( y2 >= tft.height() ) {
            h = tft.height() - y; // Clip bottom
          }
  
          // Set TFT address window to clipped image bounds
          tft.startWrite(); // Requires start/end transaction now
          tft.setAddrWindow( x, y, w, h );
  
          for ( row = 0; row < h; row++ ) { // For each scanline...
            // Seek to start of scan line.  It might seem labor-
            // intensive to be doing this on every line, but this
            // method covers a lot of gritty details like cropping
            // and scanline padding.  Also, the seek only takes
            // place if the file position actually needs to change
            // (avoids a lot of cluster math in SD library).
            if ( flip ) { // Bitmap is stored bottom-to-top order (normal BMP)
              pos = bmpImageoffset + ( bmpHeight - 1 - ( row + by1 ) ) * rowSize;
            } else {     // Bitmap is stored top-to-bottom
              pos = bmpImageoffset + ( row + by1 ) * rowSize;
            }
            pos += bx1 * 3; // Factor in starting column (bx1)
            if ( bmpFile.position() != pos ) { // Need seek?
              tft.endWrite(); // End TFT transaction
              bmpFile.seek( pos );
              buffidx = sizeof( sdbuffer ); // Force buffer reload
              tft.startWrite(); // Start new TFT transaction
            }
            for ( col=0; col<w; col++ ) { // For each pixel...
              // Time to read more pixel data?
              if ( buffidx >= sizeof( sdbuffer ) ) { // Indeed
                tft.endWrite(); // End TFT transaction
                bmpFile.read( sdbuffer, sizeof( sdbuffer ) );
                buffidx = 0; // Set index to beginning
                tft.startWrite(); // Start new TFT transaction
              }
              // Convert pixel from BMP to TFT format, push to display
              b = sdbuffer[buffidx++];
              g = sdbuffer[buffidx++];
              r = sdbuffer[buffidx++];
              tft.writePixel( tft.color565( r,g,b ) );
            } // end pixel
          } // end scanline
          tft.endWrite(); // End last TFT transaction
        } // end onscreen
        Serial.print( F( "Loaded in " ) );
        Serial.print( millis() - startTime );
        Serial.println( " ms" );
      } // end goodBmp
    }
  }

  bmpFile.close();
  if ( ! goodBmp ) {
    Serial.println( F( "BMP format not recognized." ) );
  }
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16( File &f ) {
  uint16_t result;
  ( (uint8_t *)&result )[0] = f.read(); // LSB
  ( (uint8_t *)&result )[1] = f.read(); // MSB

  return result;
}

uint32_t read32( File &f ) {
  uint32_t result;
  ( (uint8_t *)&result )[0] = f.read(); // LSB
  ( (uint8_t *)&result )[1] = f.read();
  ( (uint8_t *)&result )[2] = f.read();
  ( (uint8_t *)&result )[3] = f.read(); // MSB

  return result;
}

