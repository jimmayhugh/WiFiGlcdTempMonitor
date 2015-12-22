/*
WiFiGlcdTempMonitor - updateLCD.ino

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.0
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

void setLcdStatus(void)
{
  if(setDebug & lcdDebug)
  {
    Serial.println("Setting lcdSetStatus");
  }
  lcd.attach(lcdDelayVal, lcdSetStatus);
  lcdStatus = FALSE;
}

void lcdSetStatus(void)
{
  lcd.detach();
  lcdStatus = TRUE;
  if(setDebug & lcdDebug)
  {
    Serial.println("lcdStatus = TRUE");
  }
}


void updateLCD(void)
{
  int16_t xCursor, yCursor;

  startDebugUpdate();
  
  if(setDebug & lcdDebug)
  {
    Serial.println("Time to Write to the LCD");
  }
  
  glcd.setTextColor(ILI9341_YELLOW);
  yield();
  glcd.setTextSize(3);
  yield();
  glcd.setCursor(0, 0);
  if(setDebug & lcdDebug)
  {
    Serial.println("Writing to tft");
  }
  IPAddress ip = WiFi.localIP();
  sprintf(lcdBuffer, "IP:%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (int16_t) (320 - xCursor), (int16_t) 21, (uint16_t) ILI9341_BLACK);
  glcd.setCursor(xCursor, yCursor);
  glcd.println(lcdBuffer);
  delay(delayVal);
  
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (int16_t) (320 - xCursor), (int16_t) 21, (uint16_t) ILI9341_BLACK);
  glcd.setCursor(xCursor, yCursor);
  glcd.println(versionInfo);
  delay(delayVal);
  
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (int16_t) (320 - xCursor), (int16_t) 21, (uint16_t) ILI9341_BLACK);
  glcd.setCursor(xCursor, yCursor);
  glcd.print("Port: ");
  glcd.println(udpPort);
  delay(delayVal);
  
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (320 - xCursor), (int16_t) 21, ILI9341_BLACK);
  glcd.setCursor(xCursor, yCursor);
  glcd.println(mDNSdomain);
  glcd.println();
  delay(delayVal);

  for(uint8_t x = 0; x < 4; x++)
  {
    if(tempSet == 'F')
    {
      dtostrf( (ds18b20[x].tempVal * 1.8) + 32.0 ,7, 2, floatStr);
    }else{  
      dtostrf(ds18b20[x].tempVal,7, 2, floatStr);
    }
    floatStr = trim( (char *) floatStr);
    sprintf(lcdBuffer, "Temp%d: %s %c", x, floatStr, tempSet);
    xCursor = glcd.getCursorX();
    yCursor = glcd.getCursorY();
    glcd.fillRect(xCursor, yCursor, (320 - xCursor), (int16_t) 28, ILI9341_BLACK);
    glcd.setCursor(0, yCursor);
    glcd.println(lcdBuffer);
    if(setDebug & lcdDebug)
    {
      Serial.println(lcdBuffer);
    }
    delay(delayVal);
  }
  dBug.detach();
  setLcdStatus();
}

char *trim(char *str)
{
  char *ibuf = str, *obuf = str;
  int i = 0, cnt = 0;

  #define NUL '\0'
  /*
  **  Trap NULL
  */

  if (str)
  {
    /*
    **  Remove leading spaces (from RMLEAD.C)
    */

    for (ibuf = str; *ibuf && isspace(*ibuf); ++ibuf);

    if (str != ibuf)
      memmove(str, ibuf, ibuf - str);

    /*
    **  Collapse embedded spaces (from LV1WS.C)
    */

    while (*ibuf)
    {
      if (isspace(*ibuf) && cnt)
            ibuf++;
      else
      {
        if (!isspace(*ibuf))
              cnt = 0;
        else
        {
          *ibuf = ' ';
          cnt = 1;
        }
        obuf[i++] = *ibuf++;
      }
    }
    obuf[i] = NUL;

    /*
    **  Remove trailing spaces (from RMTRAIL.C)
    */

    while (--i >= 0)
    {
          if (!isspace(obuf[i]))
                break;
    }
    obuf[++i] = NUL;
  }
  return str;
}

