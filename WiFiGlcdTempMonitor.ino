/*
WiFiGlcdTempMonitor.ino

Version 0.0.1
Last Modified 12/12/2015
By Jim Mayhugh


Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

This software uses multiple libraries that are subject to additional
licenses as defined by the author of that software. It is the user's
and developer's responsibility to determine and adhere to any additional
requirements that may arise.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Configuration :
  The configuration is done one time, and results are stored in EEPROM
  
  (1) Enter the ssid and password of your Wifi AP.

  (2) If using a static IP, enter "n" when asked about using DHCP,
      then enter the static IP, gateway IP, and netmask, otherwise enter
      "y" to get an IP assigned

  (3) Enter the port number your server is listening on.

*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Ticker.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <EEPROMAnything.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

const char *version = "0.0.1";
const char *date    = "12/12/2015";
const char *by      = "Jim Mayhugh";
char *versionInfo = (char *)"Version 0.0.1";

int status = WL_IDLE_STATUS;
const uint8_t domainCnt = 15;
const uint8_t bonjourCnt = 50;
const uint8_t usemDNS = 0xAA;

// debug flags
const uint16_t udpDebug       = 0x0001;
const uint16_t tempDebug      = 0x0002;
const uint16_t switchDebug    = 0x0004;
const uint16_t lcdDebug       = 0x0008;
const uint16_t eepromDebug    = 0x0010;
const uint16_t findChipsDebug = 0x0020;
const uint16_t mdnsDebug      = 0x0040;
const uint16_t dBugDebug      = 0x0080;
const uint16_t loopDebug      = 0x0100;
const uint16_t ipDebug        = 0x0200;

const uint8_t  lcdBufferSize = 21;

uint16_t setDebug = 0x0000;

char packetBuffer[512]; // buffer to hold incoming and outgoing packets
char updateBuffer[128]; // buffer to hold updateStatus
char lcdBuffer[lcdBufferSize];

int16_t noBytes, packetCnt;
int16_t sDelayVal = 5000;
int16_t lowerC, lowerF, upperC, upperF;
uint32_t lowerDelay, upperDelay, tempDelay, startUpperTimer, startLowerTimer, delayVal = 10, dBugDelay = 100, lcdDelayVal = 2;
int8_t i;
uint8_t lcdCnt = 0;
uint8_t data[15];
uint8_t chip[8];
uint8_t chipStatus[3];
char tempSet;
char *delim =",";
char *result = NULL;
char mDNSdomain[domainCnt] = "ESP8266";
char bonjourBuf[bonjourCnt] = "";
uint8_t chipCnt = 0;
uint8_t mode = 0xFF, mDNSset;

// EEPROM Storage locations
const uint16_t EEPROMsize   = 4096;
const uint16_t EETemp       = 0x0008; // 'C' = Celsius 'F' = Fahrenheit
const uint16_t EETemp0      = 0x0010; // Temp0 Name
const uint16_t EETemp1      = 0x0020; // Temp1 Name
const uint16_t EETemp2      = 0x0030; // Temp2 Name
const uint16_t EETemp3      = 0x0040; // Temp3 Name
const uint16_t EEmDNSset    = 0x0050; // 0xAA = set, anything else is uninitialized
const uint16_t EEmDNSdomain = 0x0060; // mDNS domain name
const uint16_t EEWiFiSet    = 0x0080; // 0xAA = set, anything else is unitialized
const uint16_t EEssid       = 0x0090; // WiFi SSID   string
const uint16_t EEpasswd     = 0x00B0; // WiFi PASSWD string
const uint16_t EEuseUDPport = 0x00E0; // 0xAA = set, anything else is uninitialized
const uint16_t EEudpPort    = 0x00F0; // UDP port address
const uint16_t EEipSet      = 0x0140; // 0xAA = set, anything else is uninitialized
const uint16_t EEipAddress  = 0x0150; // static IP Address 
const uint16_t EEipGateway  = 0x0160; // static IP gateway
const uint16_t EEipSubnet   = 0x0170; // static IP subnet

const uint8_t useS0 = 0xAA;
const uint8_t useS1 = 0xAA;
uint8_t s0Set = 0, s1Set = 0;

// WiFi stuff
const uint8_t WiFiStrCnt = 32;  // max string length
const uint8_t ipStrCnt = 22;
const uint8_t useWiFi = 0xAA;
const uint8_t useUDPport = 0xAA;
const uint8_t udpPortCnt = 4;
const uint8_t macCnt = 6;
uint8_t wifiSet = 0, udpSet = 0;
uint8_t macAddress[macCnt] = {0,0,0,0,0,0};
char ssid[WiFiStrCnt]   = "SSID";        // your network SSID (name)
char passwd[WiFiStrCnt] = "PASSWD";      // your network password
char ipBuf[ipStrCnt] = "255,255,255,255";
char gwBuf[ipStrCnt] = "255,255,255,255";
char snBuf[ipStrCnt] = "255,255,255,255";
char udpAddr[ipStrCnt] = "2652";
uint16_t udpPort = 2652;                // local port to listen for UDP packets

const uint8_t useStaticIP = 0xAA;
const uint8_t useDHCP = 0x55;
uint8_t staticIPset = 0;

IPAddress staticIP;
IPAddress staticGateway;
IPAddress staticSubnet;

// GLCD Stuff

// ILI9341 GLCD -Ticker  lcd; For the ESP8266, these are the default.
#define TFT_DC 5
#define TFT_CS 15
#define ROTATION 3

Adafruit_ILI9341 glcd = Adafruit_ILI9341(TFT_CS, TFT_DC);

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

// A UDP instance to allow interval updates to be sent
uint16_t  statusPort;
WiFiUDP udpStatus;

char *floatStr = "               ";

// OneWire Stuff
// Family codes
const uint8_t t3tcID         = 0xAA; // Teensy 3.0 1-wire slave with MAX31855 K-type Thermocouple chip
const uint8_t dsLCD          = 0x47; // Teensy 3.x 1-wire slave 4x20 HD44780 LCD
const uint8_t dsGLCDP        = 0x45; // Teensy 3.1 1-wire slave 800x400 7" GLCD with Paging
const uint8_t dsGLCD         = 0x44; // Teensy 3.1 1-wire slave 800x400 7" GLCD
const uint8_t max31850ID     = 0x3B; // MAX31850 K-type Thermocouple chip
const uint8_t ds2762ID       = 0x30; // Maxim 2762 digital k-type thermocouple
const uint8_t ds18b20ID      = 0x28; // Maxim DS18B20 digital Thermometer device
const uint8_t ds2406ID       = 0x12; // Maxim DS2406+ digital switch


//const uint8_t oneWireAddress  =  12; // OneWire Bus Address - use pin GPIO12 for ESP-12 and ESP-07 boards
const uint8_t oneWireAddress  =   2; // OneWire Bus Address - use pin GPIO2 for ESP-01 board

const uint8_t maxChips        =   4; // Maximum number of devices
const uint8_t chipAddrSize    =   8; // 64bit OneWire Address
const uint8_t tempDataSize    =   9; // temp data
const uint8_t switchDataSize  =  13; // switch data
const uint8_t chipNameSize    =  7; // Temp Name on LCD max 11 chars
const uint16_t cDelayVal      = 900; // delay reading DS18B20

bool  tempConversion = FALSE;
bool  lcdUpdateStatus = FALSE;
bool  softReset = FALSE;
bool  udpStatusSet = FALSE;
bool  mdnsUpdateStatus = FALSE;
bool  lcdStatus = FALSE;

char tempf[10];
    
// Temp Stuff
typedef struct
{
  uint8_t     tempAddr[chipAddrSize];
  uint8_t     tempData[tempDataSize];
  float       tempVal;
  char        tempName[chipNameSize];
}tempStruct;

const tempStruct tempClear = { {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0, ""};

tempStruct ds18b20[maxChips] = { 
                                 { {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0, "Temp 0" },
                                 { {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0, "Temp 1" },
                                 { {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0, "Temp 2" },
                                 { {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0, "Temp 3" }
                               };

OneWire ds(oneWireAddress);  // on pin 2 (a 4.7K resistor is necessary)
Ticker  ds18; // timer to allow DS18B20 to be read
Ticker  lcdUpdate; // timer for LCD updates
Ticker  mdnsServer;
Ticker  lcd;
Ticker  dBug;

// mDNS stuff
// multicast DNS responder
MDNSResponder mdns;
uint16_t mdnsUpdateDelay = 10;
char bonjourNameBuf[25];


void setup(void)
{
  EEPROM.begin(EEPROMsize); // set EEPROM size first thing

  // Open serial communications and wait for port to open:
  ESP.wdtDisable(); // disable the watchdog Timer
  
  Serial.begin(115200);
  delay(sDelayVal);

  setDebug |= eepromDebug; // display eeprom state during startup
  showEEPROM();
  setDebug &= ~eepromDebug;
  yield();

  updateEEPROM(EEipSet);
  yield();
  showEEPROM();
  yield();

  glcd.begin();
  glcd.setRotation(ROTATION);
  yield();
  glcd.fillScreen(ILI9341_BLACK);
  yield();
  glcd.setTextColor(ILI9341_YELLOW);
  yield();
  glcd.setTextSize(3);
  yield();
  glcd.setCursor(0, 0);
  glcd.println("  INITIALIZING  ");
  yield();
  glcd.println("  PLEASE WAIT   ");
  yield();

  Serial.print("MAC Address = ");
  uint8_t *mac = WiFi.macAddress(macAddress);
  for(uint8_t q = 0; q < macCnt; q++)
  {
    if(mac[q] < 0x10)
      Serial.print("0");
    Serial.print(mac[q], HEX);
    if(q < (macCnt - 1))
      Serial.print(":");
  }
  Serial.println();


  if(wifiSet != useWiFi)
  {
    uint8_t z = 0;

    glcd.println("ENTER YOUR SSID,");
    yield();
    glcd.println("PASSWORD, AND IP");
    yield();
    glcd.println("ADDRESSES ON THE");
    yield();
    glcd.println(" SERIAL MONITOR ");
    
    startDebugUpdate();
    
    for(uint8_t z = 0; z < WiFiStrCnt; z++) // clear ssid and passwd string
    {
      ssid[z]   = 0xFF;
      passwd[z] = 0xFF;
    }

    Serial.print("Enter SSID:");
    fillBuf(ssid); 
    Serial.println(ssid);

    Serial.print("Enter PASSWD:");
    fillBuf(passwd); 
    Serial.println(passwd);

    wifiSet = useWiFi;
    updateEEPROM(EEWiFiSet);
    dBug.detach();
  }

  if( (staticIPset != useStaticIP) && (staticIPset != useDHCP) )
  {
    char c = 0x00;
    Serial.print("Use DHCP?:");

    startDebugUpdate();
    
    while(1)
    {
      while(Serial.available())
      {
        c = Serial.read();
        break;
      }
      if(c != 0x00)
      {
        Serial.println(c);
        break;
      }
    }

    while(Serial.available())
      Serial.read(); // flush the buffer
          
    switch(c)
    {
      case 'N':
      case 'n':
      {
        uint8_t z = 0;

        delay(100);
        while(Serial.available())
          Serial.read(); // flush the buffer

        Serial.print("Enter Static IP:");
        fillBuf(ipBuf); 
        staticIP = strToAddr(ipBuf);
        Serial.println(staticIP);

        while(Serial.available())
          Serial.read(); // flush the buffer
          
        Serial.print("Enter Static Gateway:");
        fillBuf(gwBuf); 
        staticGateway = strToAddr(gwBuf);
        Serial.println(staticGateway);

        while(Serial.available())
          Serial.read(); // flush the buffer
          
        Serial.print("Enter Static subnet:");
        fillBuf(snBuf); 
        staticSubnet = strToAddr(snBuf);
        Serial.println(staticSubnet);
        staticIPset = useStaticIP;
        updateEEPROM(EEipSet);
        
        break;
      }

      case 'Y':
      case 'y':
      default:
      {
        staticIPset = useDHCP;
        updateEEPROM(EEipSet);
        break;
      }
      
    }
  } 
        
  while(Serial.available())
    Serial.read(); // flush the buffer

  if(udpSet != useUDPport)
  {
    uint8_t z = 0;

    for(uint8_t z = 0; z < udpPortCnt; z++) // clear UDP Port Address string
    {
      udpAddr[z]   = 0xFF;
    }

    Serial.print("Enter UDP Port:");

    fillBuf(udpAddr); 
    udpPort = atoi( (char *) udpAddr);
    udpSet = useUDPport;
    Serial.println(udpPort);
    updateEEPROM(EEuseUDPport);
  }

  dBug.detach();

  // setting up Station AP
  WiFi.begin(ssid, passwd);
  
  // Wait for connect to AP
  Serial.print("[Connecting]");
  Serial.print(ssid);
  int tries=0;
  while (WiFi.status() != WL_CONNECTED)
  {
    startDebugUpdate();
    delay(500);
    yield();
    Serial.print(".");
    tries++;
    if (tries > 30)
    {
      Serial.println();
      Serial.println("Unable to Connect - Check and restart");
      yield();
      glcd.setRotation(ROTATION);
      yield();
      glcd.fillScreen(ILI9341_BLACK);
      yield();
      glcd.setTextColor(ILI9341_YELLOW);
      yield();
      glcd.setTextSize(3);
      yield();
      glcd.setCursor(0, 0);
      yield();
      glcd.println("UNABLE TO CONNECT");
      yield();
      glcd.println("  PLEASE CHECK   ");
      yield();
      glcd.println("   AND RESET     ");
      while(1)
      {
        yield();
      }
    }
    dBug.detach();
  }
  Serial.println();

  if(staticIPset == useStaticIP)
  {
    WiFi.config(staticIP, staticGateway, staticSubnet);
    Serial.print("Using Static IP - ");
  }else{
    Serial.print("Using DHCP - ");
  }
  IPAddress ip = WiFi.localIP();
  Serial.print("Connected to wifi at IP Address: ");
  Serial.println(ip);

/*
  if(ESP8266Bonjour.begin(bonjourNameBuf))
  {
    Serial.println("Bonjour Service started");
    sprintf(bonjourBuf, "%s._discover", bonjourNameBuf);
    ESP8266Bonjour.addServiceRecord(bonjourBuf, udpPort, MDNSServiceUDP);
//    I2CEEPROM_writeAnything(I2CEEPROMbjAddr, bonjourNameBuf, I2C0x50);
  }else{
    Serial.println(F("Bonjour Service failed"));
  }
  ESP8266Bonjour.run();
*/

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network


  if(mDNSset != usemDNS)
  {
    sprintf(mDNSdomain, "%s%d", mDNSdomain, ip[3]);
  }  

  if (!mdns.begin(mDNSdomain, WiFi.localIP()))
  {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  startMDNSupdate();
  Serial.print("mDNS responder started with ");
  Serial.println(mDNSdomain);

  setLcdStatus();

  if(setDebug > 0)
  {
    printWifiStatus();
  }

  Serial.print("Udp server started at port at ");
  Serial.println(udpPort);
  Udp.begin(udpPort);

  startDebugUpdate();
  findChips();
  dBug.detach();
  glcd.fillScreen(ILI9341_BLACK);
  yield();
}

void loop(void)
{
  noBytes = Udp.parsePacket();

  scanChips();

  yield();

  if(lcdStatus == TRUE)
  {
    updateLCD();
    setLcdStatus();
  }

  yield();

  if ( noBytes ) 
    processUDP();

  yield();

  if ( mdnsUpdateStatus == TRUE )
    startMDNSupdate();
}


