/*
WiFiGlcdTempMonitor - findChips.ino

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

void findChips(void)
{
  chipCnt = 0;
  while(chipCnt < maxChips)
  {
    if ( !ds.search(chip)) {
      if(setDebug & findChipsDebug)
      {
        Serial.println("No more addresses.");
        Serial.println();
      }
      ds.reset_search();
      delay(250);
      chipCnt = 0;
      return;
    }
    
    if(setDebug & findChipsDebug)
    {
      Serial.print("ROM =");
      for( i = 0; i < 8; i++)
      {
        Serial.write(' ');
        Serial.print(chip[i], HEX);
      }
    }

    if (ds.crc8(chip, 7) != chip[7])
    {
      if(setDebug & findChipsDebug)
      {
        Serial.println("CRC is not valid!");
      }
        return;
    }

    if(setDebug & findChipsDebug)
    {
      Serial.println();
    }
    
    switch(chip[0]) // set DS18B20 to 12-bit resolution
    {
      case ds18b20ID:
      {
        for(i = 0; i < chipAddrSize; i++)
          ds18b20[chipCnt].tempAddr[i] = chip[i];
        ds.reset();
        ds.select(ds18b20[chipCnt].tempAddr);
        ds.write(0x4E); // write to scratchpad;
        ds.write(0x00); // low alarm
        ds.write(0x00); // high alarm
        ds.write(0x3F); // configuration register - 12 bit accuracy (0.0625deg C)
        delay(5);
        ds.reset();
        ds.select(ds18b20[chipCnt].tempAddr);
        ds.write(0x48); // copy scratchpad to EEPROM;
        delay(5);
        if(setDebug & findChipsDebug)
        {
          Serial.println("DS18B20 resolution set to 12-bit");
        }
        for(i = 0; i < chipAddrSize; i++)
          chip[i] = 0;
        break;
      }
    }
    chipCnt++;
  }
}
