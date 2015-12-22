/*
WiFiGlcdTempMonitor - getTemp.ino

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

void getTemp(void)
{
  uint8_t x;

  if(tempConversion == FALSE)
  {
    if(setDebug & tempDebug)
      Serial.println("tempConversion is FALSE, Starting Temp Conversion");
    for( x = 0; x < maxChips; x++)
    {
      if(ds18b20[x].tempAddr[0] == 0x0)
        continue;

      ds.reset();
      ds.select(ds18b20[x].tempAddr);
      ds.write(0x44, 1);        // start conversion, with parasite power on at the end
      tempConversion = TRUE;
      tempDelay = millis();
    }
    ds18.attach_ms(cDelayVal, tempReady); //start the conversion delay
  }else{
    if(setDebug & tempDebug)
      Serial.println("tempConversion is TRUE");
  }
}

void tempReady(void)
{
  ds18.detach();
  readTemp();
}

void readTemp(void)
{
  uint8_t x;
   
  for( x = 0; x < maxChips; x++)
  {
    if(ds18b20[x].tempAddr[0] == 0x0)
      continue;

    ds.reset();
    ds.select(ds18b20[x].tempAddr);
    ds.write(0xBE);         // Read Scratchpad

    for ( i = 0; i < tempDataSize; i++)            // we need 12 int8_ts
    {
      ds18b20[x].tempData[i] = ds.read();
    }

    if(setDebug & tempDebug)
    {
      Serial.print("  Data[");
      Serial.print(x);
      Serial.print("] = ");  
      for ( i = 0; i < tempDataSize; i++)            // we need 12 int8_ts
      {
        if(ds18b20[x].tempData[i] < 0x0f)
        {
          Serial.print("0x0");
        }else{
          Serial.print("0x");
        }
        Serial.print(ds18b20[x].tempData[i], HEX);
        Serial.print(", ");
      }
      Serial.print("CRC=");
      Serial.print(ds.crc8(ds18b20[x].tempData, 8), HEX);
      Serial.println();
    }

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.

    int16_t raw = (ds18b20[x].tempData[1] << 8) | ds18b20[x].tempData[0];

    ds18b20[x].tempVal = ((float) raw / 16.0);
    
    if(setDebug & tempDebug)
    {
      Serial.print("  Temperature = ");
      Serial.print(ds18b20[x].tempVal);
      Serial.println(" Celsius");
    }
  }
  tempConversion = FALSE;
  
  if(setDebug & tempDebug)
  {
    Serial.print("tempDelay = ");
    Serial.println(millis() - tempDelay);
    Serial.println("Reading Temp");
  }
  

}

