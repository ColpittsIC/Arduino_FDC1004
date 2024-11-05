/*
  This file is part of the Arduino_FDC1004 library.
  Copyright (c) 2021 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _FDC1004_H_
#define _FDC1004_H_

#include <Arduino.h>
#include <Wire.h>

enum {
    CIN1 = 0x000,
    CIN2,
    CIN3,
    CIN4,
    CAPDAC,
    DISABLED = 0x111
};

enum {
    CHANNEL1 = 0x0,
    CHANNEL2,
    CHANNEL3,
    CHANNEL4
};

enum {
    RATE_100Ss = 0x0,
    RATE_200Ss,
    RATE_400Ss
};

enum {
    REPEAT_DISABLED = 0x0,
    REPEAT_ENABLED
};

//Define for measure enable
#define     MEAS_1_EN       (0x1 << 7)
#define     MEAS_2_EN       (0x1 << 6)
#define     MEAS_3_EN       (0x1 << 5)
#define     MEAS_4_EN       (0x1 << 4)





class FDC1004Class{
    public:
        FDC1004Class(TwoWire& wire);

        int         begin();
        void        end();
        uint16_t    measurementConfiguration(int measuresEn, int measurementRate = RATE_100Ss, int measurementRepeat = REPEAT_DISABLED);
        uint16_t    channelConfiguration(int channel,int chA,int chB, int CAPDAC);
        uint16_t    channelOffset(int channel = CHANNEL1,uint16_t offset = 0x0);
        uint16_t    channelGainConfiguration(int channel = CHANNEL1,uint16_t gain = 0x0);
        bool        measureAvailable(int channel = CHANNEL1);
        int         getChannelMeasurement(int channel = CHANNEL1);




    private:
        TwoWire*    _wire;
        uint16_t    getManufacturerID();
        uint16_t    getDeviceID();


};

extern FDC1004Class FDC1004;

#endif