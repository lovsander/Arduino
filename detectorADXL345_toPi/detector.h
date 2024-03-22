#ifndef __DETECTOR_H__
#define __DETECTOR_H__

#include <inttypes.h>
#include <Arduino.h>
#include "defines.h"
#include "debug.h"

class Detector
{
  public:
    Detector(Adafruit_ADXL345 & mpu): _mpu(mpu) {}

    uint16_t got()
    {
      return _got;
    }

    uint16_t stills()
    {
      return _stills;
    }

    uint32_t punchMax()
    {
      return _punchMax;
    }

    uint32_t punch()
    {
      return _punch;
    }

    uint32_t force()
    {
      return _force;
    }

    uint16_t ovfs()
    {
      return _ovfs;
    }

    void update()
    {
      _punch = 0;
      _punchMax = 0;
      _ovfs = 0;
      _mpu.UpdateRawAccel1();  // Read the x/y/z adc values
      _force = _mpu.calcAcc();
      bool overthresh = (_force > ACCEL_START_THRESH);
      bool continueAccum = (_cnt > 0);
      bool startAccum = (_cnt == 0);
      if (startAccum) _got = ACCEL_AXIS_THRESH;

      if (continueAccum || overthresh) // продолжаем или начинаем
      {
        // накопление
        _sum += _force;
        _maxForce = (_force > _maxForce) ? _force : _maxForce;
        if (abs(_mpu.GetRawAccX()) > ACCEL_AXIS_THRESH) _ovf[0]++;
        if (abs(_mpu.GetRawAccY()) > ACCEL_AXIS_THRESH) _ovf[1]++;
        if (abs(_mpu.GetRawAccZ()) > ACCEL_AXIS_THRESH) _ovf[2]++;

        // расчеты
        if (++_cnt >= ACCUM_SAMPLES_COUNT)
        {
          //_got = _sum;
          uint32_t avg = _sum / ACCUM_SAMPLES_COUNT;
          //_got = _maxForce;
          // собираем вектор перегрузки, разложенный по осям
          double t = 0;
          if (_ovf[0]) t += pow(_ovf[0], 2);
          if (_ovf[1]) t += pow(_ovf[1], 2);
          if (_ovf[2]) t += pow(_ovf[2], 2);
          _ovfs = round(t);
          // пока не взяли корень, это квадрат перегрузок t^2
          if (t > 0) {
            //t += sqrt(t); // а это t^2 + t
            // вся формула Евгения в одну переменную t
            //t = t / 2 / ACCUM_SAMPLES_COUNT;
            //avg += int(t * 10000);
          }
          _ovf[0] = _ovf[1] = _ovf[2] = 0;

          _cnt = 0;
          _sum = 0;
          _got = 0;
          _punch =  _maxForce; //мгновенный результат +avg +
          if (_punch > _peak) _peak = _punch;
          _maxForce = 0;
        }
      }

      if (overthresh) _belowthresh = 0;
      else if (_belowthresh < STILL_SAMPLES_COUNT) {
        _stills = ACCEL_AXIS_THRESH / 2;
        _belowthresh++;
      }
      else if (_belowthresh == STILL_SAMPLES_COUNT)
      {
        _stills = 0;
        //_got = 0;
        _punchMax = _peak; //максимальный результат
        _peak = 0;
        _belowthresh++;
      }

    }

  protected:

    Adafruit_ADXL345 & _mpu;
    uint16_t _cnt = 0;
    uint32_t _sum = 0;
    uint32_t _peak = 0;
    uint32_t _punch = 0;
    uint32_t _punchMax = 0;
    uint16_t _belowthresh = 0;
    uint32_t _ovf[3] = {0, 0, 0};
    uint32_t _force;
    uint32_t _maxForce = 0;
    uint16_t _got = 0;
    uint16_t _ovfs = 0;
    uint16_t _stills = 0;
};

#endif
