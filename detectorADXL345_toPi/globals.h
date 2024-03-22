#ifndef __GLOBALS_H__
#define __GLOBALS_H__

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

#include "debug.h"
// Акселерометер
#include "mpu.h"
//uint8_t clock, uint8_t miso, uint8_t mosi, uint8_t cs, int32_t sensorID
//scl,sdo,sda,cs,id
Adafruit_ADXL345 accel = Adafruit_ADXL345(5, 7, 6, 4, 12345); // spi
#include "detector.h"
Detector dtc1(accel);

// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ========================================
// Сила удара
word punch, punchMax, ovfs;

word punchToScore(uint32_t g_punch)
{
  uint32_t result;
  result = constrain(g_punch, LOW_PUNCH, HI_PUNCH);
  result = map(result, LOW_PUNCH, HI_PUNCH, 0, 9999);
  result = constrain(result, 0, 9999);
  return result;
}

byte CalcAMAKCRC( byte*data, int _start, int _end)
{
  byte Sum = 0;
  for ( int i = _start; i < _end; i++ )
  {
    Sum ^= data[i];
    Sum = (byte)(((Sum & 0x1) ? 0x80 : 0) | (Sum >> 1)); // rrc Sum
    if (Sum & 0x80) Sum ^= 0x3C;
  }
  return Sum;
}

String dataString() {
  //Serial.println("demo");
  String ans;
  char ansChar[5 + 32 * 2 + 1]; //70 байт
  ansChar[0] = 0xFF;
  ansChar[1] = 0x35;
  ansChar[2] = 0x41;//65 байт
  ansChar[3] = 0x00;
  ansChar[4] = 0x02;

  // 0 & 1 indexes in data array are punch & punchMax
  ansChar[5 + 0 * 2] = punch & 0xFF;
  ansChar[5 + 0 * 2 + 1] = (punch >> 8) & 0xFF;
  ansChar[5 + 1 * 2] = punchMax & 0xFF;
  ansChar[5 + 1 * 2 + 1] = (punchMax >> 8) & 0xFF;
  ansChar[5 + 2 * 2] = ovfs & 0xFF;
  ansChar[5 + 2 * 2 + 1] = (ovfs >> 8) & 0xFF;

  // reset
  //punch = 0;
  //punchMax = 0;

  int aim = -1;//random(2, 100);
  word val = 0;//random(1111, 9999);
  for (int i = 2; i < 32; i++) {
    if (i == aim) {
      ansChar[5 + i * 2] = val & 0xFF;
      ansChar[5 + i * 2 + 1] = (val >> 8) & 0xFF;
    } else  {
      ansChar[5 + i * 2] = 0;
      ansChar[5 + i * 2 + 1] = 0;
    }
  }
  ansChar[69] = CalcAMAKCRC(ansChar, 2, 68);
  for (int i = 0; i < 70; i++) {
    ans += ansChar[i];
  }
  return ans;
}

String analiz() {
  if ((byte)inputString.charAt(0) == 0xFF) {
    if ((byte)inputString.charAt(1) == 0x35) {
      digitalWrite(13, !digitalRead(13));
      byte ncmd = (byte)inputString.charAt(4);
      if (ncmd == 1) {
        return inputString;
      }
      else if (ncmd == 2)  {
        return dataString();
      }
      else return "-err1";
    }
  }
  return "-err2";
}






#endif
//*******************************************************************************************
