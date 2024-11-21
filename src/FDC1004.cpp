/*
  This file is part of the Arduino_FDC1004 library.
  Copyright (c) 2022 Arduino SA. All rights reserved.

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

#include <Wire.h>
#include "FDC1004.h"

#define FDC1004_MEAS1_MSB   0x00
#define FDC1004_MEAS1_LSB   0x01
#define FDC1004_MEAS2_MSB   0x02
#define FDC1004_MEAS2_LSB   0x03
#define FDC1004_MEAS3_MSB   0x04
#define FDC1004_MEAS3_LSB   0x05
#define FDC1004_MEAS4_MSB   0x06
#define FDC1004_MEAS5_LSB   0x07
#define FDC1004_CONF_MEAS1  0x08
#define FDC1004_CONF_MEAS2  0x09
#define FDC1004_CONF_MEAS3  0x0A
#define FDC1004_CONF_MEAS4  0x0B
#define FDC1004_FDC_CONF    0x0C
#define FDC1004_OFFSET_CIN1 0x0D
#define FDC1004_OFFSET_CIN2 0x0E
#define FDC1004_OFFSET_CIN3 0x0F
#define FDC1004_OFFSET_CIN4 0x10
#define FDC1004_GAIN_CIN1   0x11
#define FDC1004_GAIN_CIN2   0x12
#define FDC1004_GAIN_CIN3   0x13
#define FDC1004_GAIN_CIN4   0x14
#define FDC1004_MAN_ID_REG  0xFE
#define FDC1004_DEV_ID_REG  0xFF

 
#define FDC1004_ADDRESS     0x50
#define FDC1004_DEVICE_ID   0x1004


FDC1004Class::FDC1004Class(TwoWire& wire) : 
    _wire(&wire)
{}

/**
 * 
 * 
 * @param 
 * @return 
 */
uint16_t    FDC1004Class::measurementConfiguration(int measuresEn, int measurementRate, int measurementRepeat)
{
    uint16_t temp_conf_reg;
    uint8_t lsb,msb;

    temp_conf_reg = (measuresEn | ((measurementRate << 10) & 0xC00) | ((measurementRepeat << 8) & 0x100));
    msb = ((temp_conf_reg >> 8) & 0xFF);
    lsb = (temp_conf_reg & 0xFF); 
    _wire->beginTransmission(FDC1004_ADDRESS);
    _wire->write(FDC1004_FDC_CONF);
    _wire->write(msb);
    _wire->write(lsb);
    _wire->endTransmission(false);
}

/**
 * @brief Configure the Input for each channels of the FDC1004
 * 
 * @param chA       Choose what to connect to the Channel A mux 
 * @param chB       Choose what to connect to the Channel B mux 
 * @param channel   Choose the channel to configure 
 * @param CAPDAC    Negative Offset Capacitance (0-100pF). Is like a negative Capacitance
 *                  Connected to the CINn Pin
 * @return          The configured register value (Not read from IC)
 */
uint16_t    FDC1004Class::channelConfiguration(int channel,int chA,int chB, int CAPDAC)
{
    uint16_t temp_conf_reg;
    uint8_t lsb,msb;

    temp_conf_reg = ((chA << 13) & 0xE000) | ((chB << 10) & 0x1C00) | ((CAPDAC << 5) & 0x3E0);

    msb = ((temp_conf_reg >> 8) & 0xFF);
    lsb = (temp_conf_reg & 0xFF); 
    _wire->beginTransmission(FDC1004_ADDRESS);
    _wire->write(FDC1004_CONF_MEAS1 + channel);
    _wire->write(msb);
    _wire->write(lsb);
    _wire->endTransmission(false);
}

/**
 * @brief Configure Channel Offset Calibration.
 * 
 * This Calibration has a range -16pF to 16pF.
 * This should be used for fine-Tune Calibration; first a proper offset should be
 * selected with CAPDAC Offset (channelConfiguration()).
 * 
 * @param channel   Choose the channel to configure 
 * @param offset    Offset value
 * @return          The configured register value (Not read from IC)
 */
uint16_t    FDC1004Class::channelOffset(int channel,uint16_t offset)
{
    uint8_t lsb,msb;
    _wire->beginTransmission(FDC1004_ADDRESS);
    _wire->write(FDC1004_OFFSET_CIN1 + channel);
    msb = (0x00FF & (offset>>8));
    lsb = (0x00FF & offset);
    _wire->write(msb);
    _wire->write(lsb);
    _wire->endTransmission(false);
}

/**
 * @brief Configure Channel Gain Calibration.
 * 
 * This Calibration has a range 0 - 4.
 * This should be used to remove gain mismatch due to external circuitry.
 * 
 * @param channel   Choose the channel to configure 
 * @param gain      gain value
 * @return          The configured register value (Not read from IC)
 */
uint16_t    FDC1004Class::channelGainConfiguration(int channel,uint16_t gain)
{
    uint8_t lsb,msb;
    _wire->beginTransmission(FDC1004_ADDRESS);
    _wire->write(FDC1004_GAIN_CIN1 + channel);
    msb = (0x00FF & (gain>>8));
    lsb = (0x00FF & gain);
    _wire->write(msb);
    _wire->write(lsb);
    _wire->endTransmission(false);
}

/**
 * @brief Return True if a measure for the selected channel is available.
 * 
 * 
 * @param channel   The Channel to check 
 * @return          true if a measure is available; false otherwise     
 */
bool        FDC1004Class::measureAvailable(int channel)
{
    uint8_t lsb1,msb1;
    uint16_t read_reg;

    _wire->beginTransmission(FDC1004_ADDRESS);
    _wire->write(FDC1004_FDC_CONF);
    _wire->endTransmission(false);

    _wire->requestFrom(FDC1004_ADDRESS, 2); 
    msb1 = _wire->read();
    lsb1 = _wire->read();
    _wire->endTransmission(false);

    read_reg = ( (msb1 << 8) | lsb1 );
   
    if ( ( (read_reg & (0x1 << (3 - channel))) & (0x1 << (3 - channel)) ) != 0)
        return true;   
    return false;
    
    
}


/**
 * @brief Get Channel measurement.
 * 
 * This register should be read based on the Rate selected otherwise old data could be read  
 * Use measurementConfiguration() to know if a new measure is available.
 * 
 * @param channel   Choose the channel to configure 
 * @return          Capacitance Measure from the IC     
 */
int         FDC1004Class::getChannelMeasurement(int channel)
{
    uint8_t lsb1,msb1;
    uint8_t lsb2,msb2;
    _wire->beginTransmission(FDC1004_ADDRESS);
    _wire->write(FDC1004_MEAS1_MSB + (channel*2) );
    _wire->endTransmission(false);

    _wire->requestFrom(FDC1004_ADDRESS, 4); 
    msb1 = _wire->read();
    lsb1 = _wire->read();
    msb2 = _wire->read();
    lsb2 = _wire->read();
    _wire->endTransmission(false);

    return ((msb1 <<24) + (lsb1 <<16) + (msb2<<8) + lsb2);

}


/**
 * @brief       Return the Manufacturer ID
 * 
 * @param 
 * @return      Manufacturer ID 
 */
uint16_t    FDC1004Class::getManufacturerID()
{   uint8_t lsb,msb;
    uint16_t manID;

    _wire->beginTransmission(FDC1004_ADDRESS);
    _wire->write(FDC1004_MAN_ID_REG);
    _wire->endTransmission(false);

    _wire->requestFrom(FDC1004_ADDRESS, 2); 
    if (_wire->available() != 2) 
        return 0;

    msb = _wire->read();// MSB
    lsb = _wire->read();// LSB
    _wire->endTransmission(false);
    manID = ( msb << 8 ) | lsb ;
    return manID;
  
}

/**
 * @brief       Reeturn the Device ID
 * 
 * @param   
 * @return      Device ID
 */     
uint16_t    FDC1004Class::getDeviceID()
{
    uint8_t lsb,msb;
    uint16_t devID;

    _wire->beginTransmission(FDC1004_ADDRESS);
    _wire->write(FDC1004_DEV_ID_REG);
    _wire->endTransmission(false);

    _wire->requestFrom(FDC1004_ADDRESS, 2); 
    if (_wire->available() != 2) 
        return 0;

    msb = _wire->read();// MSB
    lsb = _wire->read();// LSB
    _wire->endTransmission(false);
    devID = ( msb << 8 ) | lsb ;
    return devID;
}


/**
 * Begin function
 * 
 * @param 
 * @return 0 if failed; 1 if everything is ok
 */
int     FDC1004Class::begin()
{
    _wire->begin();
    if (getDeviceID() != FDC1004_DEVICE_ID)
        return 0;
    return 1;
}

/**
 * End Function
 * 
 * @param 
 * @return 
 */
void    FDC1004Class::end()
{
    /*To Add: Disable all channel*/ 
    _wire->end();
}


void    FDC1004Class::resetRequest()
{
    uint16_t temp_conf_reg;
    uint8_t lsb,msb;

    temp_conf_reg = CHIP_RESET;
    msb = ((temp_conf_reg >> 8) & 0xFF);
    lsb = (temp_conf_reg & 0xFF); 
    _wire->beginTransmission(FDC1004_ADDRESS);
    _wire->write(FDC1004_FDC_CONF);
    _wire->write(msb);
    _wire->write(lsb);
    _wire->endTransmission(false);
}

FDC1004Class FDC1004(Wire1);
