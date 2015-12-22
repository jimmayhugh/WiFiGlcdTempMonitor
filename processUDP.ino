/*
WiFiGlcdTempMonitor - processUDP.ino

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

void processUDP(void)
{
  char *temp[4] = { "          ", "          ", "          ", "          " };

  startDebugUpdate();
  
  if(setDebug & udpDebug)
  {
    Serial.print(millis() / 1000);
    Serial.print(":Packet of ");
    Serial.print(noBytes);
    Serial.print(" received from ");
    Serial.print(Udp.remoteIP());
    Serial.print(":");
    Serial.println(Udp.remotePort(), HEX);
  }
  
  // We've received a packet, read the data from it
  Udp.read(packetBuffer,noBytes); // read the packet into the buffer

  // display the packet contents in HEX
  if(setDebug & udpDebug)
  {
    for (int i = 1;i <= noBytes; i++)
    {
      Serial.print(packetBuffer[i-1]);
      if (i % 32 == 0)
      {
        Serial.println();
      }
    } // end for
    Serial.println();
  }

  switch(packetBuffer[0])
  {

    case 'C':
    {
      uint8_t x = (packetBuffer[1] & 0x03);
      uint8_t y;
      
      for(y = 0; y < chipNameSize; y++) // clear Temp name buffer
        ds18b20[x].tempName[y] = 0;
      for(y = 0; y < chipNameSize; y++)
      {
        if(isalnum(packetBuffer[y+2])) // move Temp name to chip name
          ds18b20[x].tempName[y] = packetBuffer[y+2];
      }
      updateEEPROM(((uint16_t) x+1) << 4);
      packetCnt = sprintf(packetBuffer, "temp[%d] = %s", x, ds18b20[x].tempName);
      break;
    }

    case 'D':
    {
      if(setDebug & udpDebug)
        Serial.print("Setting setDebug to 0x");
      setDebug = atoi(&packetBuffer[1]);
      Serial.println(setDebug, HEX);
      packetCnt = sprintf(packetBuffer, "setDebug = %0X", setDebug);
      break;
    }

    case 'N':
    {
      uint8_t setDomain = 0, y;

      if((packetBuffer[1] == ' ') && (isalnum(packetBuffer[2])) )
      {
        for(y = 0; y < domainCnt; y++)
        {
          if(isalnum(packetBuffer[y+2]))
          {
            if(setDebug & udpDebug)
              Serial.print(packetBuffer[y+2]);
            continue;
          }else if( ((packetBuffer[y+2] == 0x00) || 
                    (packetBuffer[y+2] == 0x0A) ||
                    (packetBuffer[y+2] == 0x0D)) &&
                    y > 0
                  ){
            if(setDebug & udpDebug)
              Serial.println();
            setDomain = 1;
            break;
          }
        }

        if(setDomain == 1)
        {
          mDNSset = usemDNS;
          for(y = 0; y < domainCnt; y++) // clear domain name buffer
            mDNSdomain[y] = 0;
          for(y = 0; y < domainCnt; y++)
          {
            if(isalnum(packetBuffer[y+2])) // move domain name to domain buffer
              mDNSdomain[y] = packetBuffer[y+2];
          }
          updateEEPROM(EEmDNSset);
          softReset = TRUE;
        }
      }
      packetCnt = sprintf(packetBuffer, "mDNSdomain = %s", mDNSdomain);
      break;
    }

    case 'T':
    {
      switch(packetBuffer[1])
      {
        case 'C':
        case 'c':
        {
          tempSet = 'C';
          if(setDebug & udpDebug)
            Serial.println("tempSet set to C");
          break;
        }

        case 'F':
        case 'f':
        {
          tempSet = 'F';
          if(setDebug & udpDebug)
            Serial.println("tempSet set to F");
          break;
        }

      }
      updateEEPROM(EETemp);
      packetCnt = sprintf(packetBuffer, "tempSet = %c", tempSet);
      break;
    }

    case 'V':
    {
      packetCnt = sprintf(packetBuffer, "Version %s, %s, by %s", version, date, by);
      break;
    }

    default:
    {
      packetCnt = 0;
      for (uint8_t x = 0; x < 4; x++)
      {
        if(tempSet == 'F')
        {
          dtostrf(((ds18b20[x].tempVal * 1.8) + 32.0), 4, 2, temp[x]);
        }else{
          dtostrf(ds18b20[x].tempVal, 4, 2, temp[x]);
        }
        if(setDebug & udpDebug)
        {
          Serial.print("temp[");
          Serial.print(x);
          Serial.print("]=");
          Serial.println(temp[x]);
        }
        if(x>=3)
        {
          packetCnt += sprintf(packetBuffer+packetCnt, "%s,%s", ds18b20[x].tempName, temp[x]);
        }else{
          packetCnt += sprintf(packetBuffer+packetCnt, "%s,%s,", ds18b20[x].tempName, temp[x]);
        }
      }
      break;
    }
  }

  if(setDebug & udpDebug)
    Serial.println(packetBuffer);
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write(packetBuffer, packetCnt);
  Udp.endPacket();
  dBug.detach();
}
