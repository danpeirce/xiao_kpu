/*
  xiao_kpu.ino
  
  Modified from 
  LoadFromSD.ino for use uith a Seeed XIAO ATSAMD21 board with expansion board.
  Original example was for general use and hundreds of lines have been removed for this 
  modified example.

  Demonstrate how to read a XBM file from SD card.
  
   
  There is also an external conversion tool, which generates the expected binary from a PNG file:
    https://github.com/olikraus/u8g2/tree/master/tools/png2bin

  The Conversion tool used to create kpu.bin whiche was copied to MicroSD before this program was run.

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2019, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
  This is a page buffer example.    
*/

U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

// This procedure will draw a bitmap at the given position x,y
// The bitmap is loaded on the fly from a file "filename" from SD card.

void drawFile(u8g2_int_t x, u8g2_int_t y, const char *filename) {
  uint8_t w;
  uint8_t h;
  uint8_t b;
  uint8_t mask;
  uint8_t len;
  u8g2_int_t xpos;
  File myFile = SD.open(filename);
  if (myFile) {
    w = myFile.read();        // read the dimension of the bitmap
    h = myFile.read();
    while( h > 0 ) {          // handle all lines of the bitmap
      xpos = x;
      len = w;                // len will contain the horizontal number of pixel
      mask = 1;
      b = myFile.read();      // load the first 8 pixel into "b"
      for(;;) {        // draw all pixel of one line
        if ( b & mask ) {     // check one pixel
          u8g2.setDrawColor(1);
          u8g2.drawPixel(xpos,y);
        } else {
          u8g2.setDrawColor(0);
          u8g2.drawPixel(xpos,y);
        }
        xpos++;               // calculate next x pos of the pixel
        mask <<= 1;           // update the mask
        len--;                // decrease the horizontal width (remaining pixel)
        if ( len == 0 )    // check if row is finished
            break;
        if ( mask == 0 )
        {
          mask = 1;           // revert mask and ...
          b = myFile.read();  // ... load the next 8 pixel values from the file
        }
      }      
      
      y++;                    // goto next line
      h--;                    // decrease the number of remaining lines
    }
    myFile.close();           // all done, close the file
  }
  u8g2.setDrawColor(1);       // restore color
}

void setup() {
  Serial.begin(9600);  // baud rate only place holder as the XIAO SAND21 board uses no UART
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // pinMode(D2, OUTPUT);
  if (!SD.begin(2)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  u8g2.begin();
  u8g2.clearDisplay();
  u8g2.setFont(u8g2_font_helvR10_tr);
  u8g2.firstPage();
  do {
    drawFile(0, 0, "kpu.bin");
    u8g2.setCursor(60, 15);
    u8g2.print(F("PHYS1600"));
    u8g2.setCursor(64, 30);
    u8g2.print(F("PMT"));
  } while ( u8g2.nextPage() );
}


void loop() {

  delay(10000);
}

