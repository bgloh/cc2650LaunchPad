/*******************************************************************************
*  Filename:       sensor_hdc1000.c
*  Revised:        $Date: 2014-02-05 10:47:02 +0100 (on, 05 feb 2014) $
*  Revision:       $Revision: 12066 $
*
*  Description:    Driver for TI HCD1000 humidity sensor
*
*  Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/* -----------------------------------------------------------------------------
*                                          Includes
* ------------------------------------------------------------------------------
*/
#include "sensor_hdc1000.h"
#include "sensor.h"
#include "bsp_i2c.h"

/* -----------------------------------------------------------------------------
*                                           Constants
* ------------------------------------------------------------------------------
*/
// Sensor I2C address
#define SENSOR_I2C_ADDRESS         0x43				// SensorTag 0x43

// Registers
#define HDC1000_REG_TEMP           0x00 // Temperature
#define HDC1000_REG_HUM            0x01 // Humidity
#define HDC1000_REG_CONFIG         0x02 // Configuration
#define HDC1000_REG_SERID_H        0xFB // Serial ID high
#define HDC1000_REG_SERID_M        0xFC // Serial ID middle
#define HDC1000_REG_SERID_L        0xFD // Serial ID low
#define HDC1000_REG_MANF_ID        0xFE // Manufacturer ID
#define HDC1000_REG_DEV_ID         0xFF // Device ID

// Fixed values
#define HDC1000_VAL_MANF_ID        0x5449
#define HDC1000_VAL_DEV_ID         0x1000
#define HDC1000_VAL_CONFIG         0x1000 // 14 bit, acquired in sequence

// Sensor selection/de-selection
#define SENSOR_SELECT()     bspI2cSelect(BSP_I2C_INTERFACE_0,SENSOR_I2C_ADDRESS)
#define SENSOR_DESELECT()   bspI2cDeselect()

/* -----------------------------------------------------------------------------
*                                           Type Definitions
* ------------------------------------------------------------------------------
*/
typedef struct
{
  uint16_t temp;
  uint16_t hum;
} SensorData_t;

/* -----------------------------------------------------------------------------
*                                           Local Functions
* ------------------------------------------------------------------------------
*/

/* -----------------------------------------------------------------------------
*                                           Local Variables
* ------------------------------------------------------------------------------
*/
static bool  success;
static SensorData_t data;

/*******************************************************************************
* @fn          sensorHdc1000Init
*
* @brief       Initialize the humidity sensor driver
*
* @return      true if I2C operation successful
*******************************************************************************/
bool sensorHdc1000Init(void)
{
  uint16_t val;

  if (!SENSOR_SELECT())
    return false;

  // Enable reading data in one operation
  val = SWAP(HDC1000_VAL_CONFIG);
  success = sensorWriteReg(HDC1000_REG_CONFIG,(uint8_t*)&val,2);

  SENSOR_DESELECT();

  return success;
}

/*******************************************************************************
* @fn          sensorHdc1000Start
*
* @brief       Start measurement
*
* @return      none
*/
void sensorHdc1000Start(void)
{
  if (success)
  {
    if (!SENSOR_SELECT())
      return;

    success = bspI2cWriteSingle(HDC1000_REG_TEMP);
    SENSOR_DESELECT();
  }
}


/*******************************************************************************
* @fn          sensorHdc1000Read
*
* @brief       Get humidity sensor data
*
* @param       rawTemp - raw temperature value
*
* @param       rawHum - raw humidity value
*
* @return      true if I2C operations successful
*/
bool sensorHdc1000Read(uint16_t *rawTemp, uint16_t *rawHum)
{
  bool valid;

  if (success)
  {
    if (!SENSOR_SELECT())
      return false;

    success = bspI2cRead((uint8_t*)&data,sizeof(data));
    SENSOR_DESELECT();

    // Store temperature
    *rawTemp = SWAP(data.temp);

    // Store humidity
    *rawHum = SWAP(data.hum);
  }

  valid = success;
  success = true; // Ready for next cycle

  return valid;
}

/*******************************************************************************
* @fn          sensorHdc1000Convert
*
* @brief       Convert raw data to temperature and humidity
*
* @param       rawTemp - raw temperature value
*
* @param       rawHum - raw humidity value
*
* @param       temp - converted temperature
*
* @param       hum - converted humidity
*
* @return      none
*******************************************************************************/
void sensorHdc1000Convert(uint16_t rawTemp, uint16_t rawHum,
                        float *temp, float *hum)
{
  //-- calculate temperature [�C]
  *temp = ((double)(int16_t)rawTemp / 65536)*165 - 40;

  //-- calculate relative humidity [%RH]
  *hum = ((double)rawHum / 65536)*100;
}


/*******************************************************************************
* @fn          sensorHdc1000Test
*
* @brief       Humidity sensor self test
*
* @return      true if test passes
*******************************************************************************/
bool sensorHdc1000Test(void)
{
  uint16_t val;

  SENSOR_SELECT();

  // Verify manufacturer ID
  ST_ASSERT(sensorReadReg(HDC1000_REG_MANF_ID,(uint8_t*)&val,2));
  val = SWAP(val);
  ST_ASSERT(val == HDC1000_VAL_MANF_ID);

  // Verify device ID
  ST_ASSERT(sensorReadReg(HDC1000_REG_DEV_ID,(uint8_t*)&val,2));
  val = SWAP(val);
  ST_ASSERT(val == HDC1000_VAL_DEV_ID);
  SENSOR_DESELECT();

  return true;
}
