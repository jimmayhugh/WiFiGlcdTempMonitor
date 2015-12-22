/*
WiFiGlcdTempMonitor - printWifiStatus.ino

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
*/

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void strToIP(uint8_t *ipArray, char *str)
{
  ipArray[0] = (uint8_t) atoi(str);
  ipArray[1] = (uint8_t) atoi((char *) &str[4]);
  ipArray[2] = (uint8_t) atoi((char *) &str[8]);
  ipArray[3] = (uint8_t) atoi((char *) &str[12]);
}

IPAddress strToAddr(char *addrStr)
{
  IPAddress addr;
  char * p;

  addr[0] = atoi(addrStr);
  p = strchr(addrStr, '.');
  addr[1] = atoi(p+1);
  p = strchr(p+1, '.');
  addr[2] = atoi(p+1);
  p = strchr(p+1, '.');
  addr[3] = atoi(p+1);
  return addr;
}

void fillBuf(char * buf)
{
  uint8_t z = 0;

  while(1)
  {
    while(Serial.available())
    {
      buf[z] = Serial.read();
      if( (buf[z] == 0x0A) || (buf[z] == 0x0D) || (buf[z] == 0x00) )
      {
        buf[z] = 0x00;
        break;
      }
      z++;
      if(z >= ipStrCnt)
      {
        buf[z] = 0x00;
        break;
      } 
    }
    if(buf[z] == 0x00)
      break;
  }
  if(setDebug & ipDebug)
  {
    Serial.print("buf = ");
    Serial.println(buf);
  }
}

