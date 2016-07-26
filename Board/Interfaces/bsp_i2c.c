/*******************************************************************************
*  Filename:       bsp_i2c.c
*  Revised:        $Date: $
*  Revision:       $Revision: $
*
*  Description:    Layer added on top of RTOS driver for backward
*                  compatibility with non RTOS I2C driver.
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
*
*******************************************************************************/
#ifdef TI_DRIVERS_I2C_INCLUDED

/*******************************************************************************
 * INCLUDES
 */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/family/arm/cc26xx/Power.h>

#include <ti/drivers/i2c/I2CCC26XX.h>

#include <driverlib/prcm.h>

#include "Board.h"
#include "sensor.h"

#include "bsp_i2c.h"

/*******************************************************************************
 * CONSTANTS
 */
#define I2C_TIMEOUT 2500

/*******************************************************************************
 * GLOBAL variables
 */
extern I2CCC26XX_HWAttrs i2cCC26xxHWAttrs[];

/*******************************************************************************
 * LOCAL variables
 */
static volatile uint8_t slaveAddr;
static volatile uint8_t interface;
static I2C_Handle i2cHandle;
static I2C_Params i2cParams;
static Semaphore_Struct mutex;
static const I2CCC26XX_I2CPinCfg pinCfg1 =
{
   // Pin configuration for I2C interface 1
  .pinSDA = Board_I2C0_SDA1,
  .pinSCL = Board_I2C0_SCL1
};


/*******************************************************************************
 * @fn          bspI2cWrite
 *
 * @brief       Burst write to an I2C device
 *
 * @param       data - pointer to data buffer
 * @param       len - number of bytes to write
 *
 * @return      true if success
 */
bool bspI2cWrite(uint8_t *data, uint8_t len)
{
  I2C_Transaction masterTransaction;

  masterTransaction.writeCount   = len;
  masterTransaction.writeBuf     = data;
  masterTransaction.readCount    = 0;
  masterTransaction.readBuf      = NULL;
  masterTransaction.slaveAddress = slaveAddr;

  return I2C_transfer(i2cHandle, &masterTransaction) == TRUE;
}


/*******************************************************************************
 * @fn          bspI2cWriteSingle
 *
 * @brief       Single byte write to an I2C device
 *
 * @param       data - byte to write
 *
 * @return      true if success
 */
bool bspI2cWriteSingle(uint8_t data)
{
  uint8_t d;

  d = data;

  return bspI2cWrite(&d, 1);
}


/*******************************************************************************
 * @fn          bspI2cRead
 *
 * @brief       Burst read from an I2C device
 *
 * @param       data - pointer to data buffer
 * @param       len - number of bytes to write
 *
 * @return      true if success
 */
bool bspI2cRead(uint8_t *data, uint8_t len)
{
  I2C_Transaction masterTransaction;

  masterTransaction.writeCount   = 0;
  masterTransaction.writeBuf     = NULL;
  masterTransaction.readCount    = len;
  masterTransaction.readBuf      = data;
  masterTransaction.slaveAddress = slaveAddr;

  return I2C_transfer(i2cHandle, &masterTransaction) == TRUE;
}

/*******************************************************************************
 * @fn          bspI2cWriteRead
 *
 * @brief       Burst write/read from an I2C device
 *
 * @param       wdata - pointer to write data buffer
 * @param       wlen - number of bytes to write
 * @param       rdata - pointer to read data buffer
 * @param       rlen - number of bytes to read
 *
 * @return      true if success
 */
bool bspI2cWriteRead(uint8_t *wdata, uint8_t wlen, uint8_t *rdata, uint8_t rlen)
{
  I2C_Transaction masterTransaction;

  masterTransaction.writeCount   = wlen;
  masterTransaction.writeBuf     = wdata;
  masterTransaction.readCount    = rlen;
  masterTransaction.readBuf      = rdata;
  masterTransaction.slaveAddress = slaveAddr;

  return I2C_transfer(i2cHandle, &masterTransaction) == TRUE;
}


/*******************************************************************************
 * @fn          bspI2cSelect
 *
 * @brief       Select an I2C interface and slave
 *
 * @param       newInterface - selected interface
 * @param       address - slave address
  *
 * @return      true if success
 */
bool bspI2cSelect(uint8_t newInterface, uint8_t address)
{
  // Acquire I2C resource
  if (!Semaphore_pend(Semaphore_handle(&mutex),MS_2_TICKS(I2C_TIMEOUT)))
  {
    return false;
  }

  // Store new slave address
  slaveAddr = address;

  // Interface changed ?
  if (newInterface != interface)
  {
    // Store new interface
    interface = newInterface;

    // Shut down RTOS driver
    I2C_close(i2cHandle);

    // Sets custom to NULL, selects I2C interface 0
    I2C_Params_init(&i2cParams);

    // Assign I2C data/clock pins according to selected I2C interface 1
    if (interface == BSP_I2C_INTERFACE_1)
    {
      i2cParams.custom = (void*)&pinCfg1;
    }

    // Re-open RTOS driver with new bus pin assignment
    i2cHandle = I2C_open(Board_I2C, &i2cParams);
  }

  return true;
}

/*******************************************************************************
 * @fn          bspI2cDeselect
 *
 * @brief       Allow other tasks to access the I2C driver
 *
 * @param       none
 *
 * @return      none
 */
void bspI2cDeselect(void)
{
  // Release I2C resource
  Semaphore_post(Semaphore_handle(&mutex));
}


/*******************************************************************************
 * @fn          bspI2cInit
 *
 * @brief       Initialize the RTOS I2C driver (must be called only once)
 *
 * @param       none
 *
 * @return      none
 */
void bspI2cInit(void)
{
  Semaphore_Params semParamsMutex;

  // Create protection semaphore
  Semaphore_Params_init(&semParamsMutex);
  semParamsMutex.mode = Semaphore_Mode_BINARY;
  Semaphore_construct(&mutex, 1, &semParamsMutex);

  // Reset the I2C controller
  HapiResetPeripheral(PRCM_PERIPH_I2C0);

  I2C_init();
  I2C_Params_init(&i2cParams);
  i2cParams.bitRate = I2C_400kHz;
  i2cHandle = I2C_open(Board_I2C, &i2cParams);

  // Initialize local variables
  slaveAddr = 0xFF;
  interface = BSP_I2C_INTERFACE_0;

  if (i2cHandle == NULL)
  {
    Task_exit();
  }
}

/*******************************************************************************
 * @fn          bspI2cReset
 *
 * @brief       Reset the RTOS I2C driver
 *
 * @param       none
 *
 * @return      none
 */
void bspI2cReset(void)
{
  // Acquire I2C resource */
  if (!Semaphore_pend(Semaphore_handle(&mutex),MS_2_TICKS(I2C_TIMEOUT)))
  {
    return;
  }

  // Close the driver
  I2C_close(i2cHandle);

  // Reset the I2C controller
  HapiResetPeripheral(PRCM_PERIPH_I2C0);

  // Reset local variables
  slaveAddr = 0xFF;
  interface = BSP_I2C_INTERFACE_0;

  // Open driver
  i2cHandle = I2C_open(Board_I2C, &i2cParams);

  // Release I2C resource
  Semaphore_post(Semaphore_handle(&mutex));
}

#endif
