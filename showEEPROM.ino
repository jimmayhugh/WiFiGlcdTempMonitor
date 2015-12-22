/*
WiFiGlcdTempMonitor - showEEPROM.ino

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

void showEEPROM(void)
{

  EEPROM_readAnything(EETemp, tempSet);

  EEPROM_readAnything(EETemp0, ds18b20[0].tempName);
  EEPROM_readAnything(EETemp1, ds18b20[1].tempName);
  EEPROM_readAnything(EETemp2, ds18b20[2].tempName);
  EEPROM_readAnything(EETemp3, ds18b20[3].tempName);

  for(uint8_t x = 0; x < 4; x++)
  {
    if(!(isalnum(ds18b20[x].tempName[0])))
      sprintf(ds18b20[x].tempName, "Temp %d", x);
  }

  EEPROM_readAnything(EEmDNSset, mDNSset);
  EEPROM_readAnything(EEipSet, staticIPset);
  if(staticIPset == useStaticIP)
  {
    EEPROM_readAnything(EEipAddress, staticIP);
    EEPROM_readAnything(EEipGateway, staticGateway);
    EEPROM_readAnything(EEipSubnet, staticSubnet);
  }
  if(mDNSset == usemDNS)  
    EEPROM_readAnything(EEmDNSdomain, mDNSdomain);

  EEPROM_readAnything(EEWiFiSet, wifiSet);
  if(wifiSet == useWiFi)
  {
    EEPROM_readAnything(EEssid, ssid);
    EEPROM_readAnything(EEpasswd, passwd);
  }

  EEPROM_readAnything(EEuseUDPport, udpSet);
  if(udpSet == useUDPport)
  {
    EEPROM_readAnything(EEudpPort, udpPort);
  }  

  if(setDebug & eepromDebug)
  {
    Serial.print("tempSet = ");
    Serial.println(tempSet);
    for(uint8_t x = 0; x < 4; x++)
    { 
      Serial.print("Temp");
      Serial.print(x);
      Serial.print(" = ");
      Serial.println(ds18b20[x].tempName);
    }
    Serial.print("mDNSdomain = ");
    Serial.println(mDNSdomain);
    delay(100);
    Serial.print("ssid = ");
    Serial.println(ssid);
    delay(100);
    Serial.print("passwd = ");
    Serial.println(passwd);
    delay(100);
    Serial.print("staticIP = ");
    Serial.println(staticIP);
    delay(100);
    Serial.print("staticGateway = ");
    Serial.println(staticGateway);
    delay(100);
    Serial.print("staticSubnet = ");
    Serial.println(staticSubnet);
    delay(100);
    Serial.print("udpPort = ");
    Serial.println(udpPort);
  }

}

void updateEEPROM(uint16_t level)
{
  switch(level)
  {
    case EETemp:
    {
      EEPROM_writeAnything(EETemp, tempSet);
      break;
    }

    case EETemp0:
    {
      EEPROM_writeAnything(EETemp0, ds18b20[0].tempName);
      break;
    }
    
    case EETemp1:
    {
      EEPROM_writeAnything(EETemp1, ds18b20[1].tempName);
      break;
    }
    
    case EETemp2:
    {
      EEPROM_writeAnything(EETemp2, ds18b20[2].tempName);
      break;
    }
    
    case EETemp3:
    {
      EEPROM_writeAnything(EETemp3, ds18b20[3].tempName);
      break;
    }
    
    case EEmDNSset:
    {
      EEPROM_writeAnything(EEmDNSset, mDNSset);
      EEPROM_writeAnything(EEmDNSdomain, mDNSdomain);
      break;
    }

    case EEWiFiSet:
    {
      EEPROM_writeAnything(EEWiFiSet, wifiSet);
      EEPROM_writeAnything(EEssid, ssid);
      EEPROM_writeAnything(EEpasswd, passwd);
      break;
    }

    case EEipSet:
    {
      EEPROM_writeAnything(EEipSet, staticIPset);
      for(uint8_t x = 0; x < 4; x++)
      {
        EEPROM_writeAnything(EEipAddress, staticIP);
        EEPROM_writeAnything(EEipGateway, staticGateway);
        EEPROM_writeAnything(EEipSubnet, staticSubnet);
      }
      break;
    }

    case EEuseUDPport:
    {
      EEPROM_writeAnything(EEuseUDPport, udpSet);
      EEPROM_writeAnything(EEudpPort, udpPort);
      break;
    }
  }
  EEPROM.commit();
  if(setDebug & eepromDebug)
    Serial.println("updateEEPROM Completed");
  showEEPROM();
}
