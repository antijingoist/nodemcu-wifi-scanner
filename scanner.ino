#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "certs.h"
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 12, /* reset=*/ U8X8_PIN_NONE);   // inst

void setup(void) {
  Serial.begin(115200);
  u8g2.begin();

  u8g2.clearBuffer();	
  u8g2.setFont(u8g2_font_NokiaSmallPlain_tr);
  u8g2.drawStr(0,10,"Scanning WiFi Networks ...");
  u8g2.setFont(u8g2_font_open_iconic_app_4x_t);
  u8g2.drawStr(46,62,"@");
    u8g2.drawHLine(0, 13, 128);
    u8g2.drawHLine(0, 15, 128);
    u8g2.drawHLine(127, 14, 128);
  u8g2.sendBuffer();

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);
  // Disconnect from an AP if it was previously connected
  WiFi.disconnect();

  // give it a little time to clear and start, but show progress while doing it.
    delay(10);
    for( int sx = 0; sx <= 10; sx++){
      u8g2.drawHLine(0, 14, (int)(sx * 12.7));
      u8g2.sendBuffer();
    }
}

void loop(void) {
  // Look and Feel
  const unsigned char* TitleFont = u8g2_font_NokiaSmallPlain_tr;
  const unsigned char* DetailFont  = u8g2_font_micro_tr;
  u8g2.setFont( DetailFont ); 

  int pxlStart      = 15; // Title area ends 1px before this
  int titleBaseline = pxlStart - 4;
  int lineHeight    = u8g2.getMaxCharHeight() + 1;
  int pageResults   = (int)((u8g2.getDisplayHeight() - pxlStart) / lineHeight) - 1;

  // WiFi Vars
  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;
  bool hidden;
  int scanResult = -2;


  int currentLine = 0;
  int resultsLine = 1;
  int pages = 0;

// Paginate results
  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
  if((scanResult % pageResults)) {
    pages = (int)(scanResult / pageResults) + 1;
  } else {
    pages = (int)(scanResult / pageResults);
  }

  u8g2.clearBuffer();	
  u8g2.setFont( TitleFont );
  u8g2.setFontMode( 0 );

  if (scanResult == 0) {
    u8g2.drawStr(0,25,"No networks detected.");	
    u8g2.sendBuffer();
  } else if (scanResult == -2) {
    u8g2.drawStr(0,10,"Scanning WiFi Networks");
    u8g2.sendBuffer();
  } else if (scanResult > 0) { 
    for (int8_t p = 1; p <= pages; p++) {
      u8g2.clearBuffer();	
      // Draw Header & info
      u8g2.drawHLine(1,   0,  126);

      // progress bar
      u8g2.drawHLine(0,  15,  128);
      u8g2.drawHLine(0,  13,  128);

      u8g2.drawVLine(0,   1,   14);
      u8g2.drawVLine(127, 1,   14);

      u8g2.setFont( TitleFont );
        u8g2.drawStr(  3, titleBaseline, String(scanResult).c_str());
        u8g2.drawStr( 20, titleBaseline, "Networks Found");
        u8g2.drawStr( 99, titleBaseline, String(p).c_str());
        u8g2.drawStr(109, titleBaseline, "/");
        u8g2.drawStr(119, titleBaseline, String(pages).c_str());
      u8g2.setFont( DetailFont );

      // Page results
      for (int8_t i = 0; i <= pageResults; i++) {
        WiFi.getNetworkInfo(currentLine, ssid, encryptionType, rssi, bssid, channel, hidden);
        const bss_info *bssInfo = WiFi.getScanInfoByIndex(i);
        String phyMode;
        const char *wps = "";
        if (bssInfo) {
          phyMode.reserve(12);
          phyMode = F("");
          String slash;
          if (bssInfo->phy_11b) {
            phyMode += 'b';
            slash = '/';
          }
          if (bssInfo->phy_11g) {
            phyMode += slash + 'g';
            slash = '/';
          }
          if (bssInfo->phy_11n) {
            phyMode += slash + 'n';
          }
          if (bssInfo->wps) {
            wps = PSTR("WPS");
          }
        }



        ssid.trim();
        if(currentLine < scanResult) {
          if( hidden ) {
            u8g2.drawStr(0, (((i + resultsLine) * lineHeight) + pxlStart), "?? Hidden ??");
            u8g2.drawStr((125 - u8g2.getStrWidth(String(phyMode).c_str())), (((i + resultsLine) * lineHeight) + pxlStart), phyMode.c_str());
          } else {
            u8g2.drawStr(0, (((i + resultsLine) * lineHeight) + pxlStart), ssid.c_str());
            u8g2.drawStr((125 - u8g2.getStrWidth(String(phyMode).c_str())), (((i + resultsLine) * lineHeight) + pxlStart), phyMode.c_str());
          }
        }
        currentLine++;
        yield();
      }
      u8g2.sendBuffer();
      // Show timer status to switch to next page. 
      // Blink for awareness
      for(int r = 1; r < 10; r++) {
        for (int b = 0; b < 5; b++) {
          u8g2.setDrawColor(1);
          u8g2.drawHLine((((r-1) * 14) + 1), 14, 14);
          u8g2.sendBuffer();
          delay(100);
          u8g2.setDrawColor(0);
          u8g2.drawHLine((((r-1) * 14) + 1), 14, 14);
          u8g2.sendBuffer();
          delay(100);
          yield();
        }
        u8g2.setDrawColor(1);
        u8g2.drawHLine((((r-1) * 14) + 1), 14, 14);
        u8g2.sendBuffer();
      }
    }
  } 
  else {
    u8g2.drawStr(0,10, "Error.");
    u8g2.sendBuffer();
  }
}

