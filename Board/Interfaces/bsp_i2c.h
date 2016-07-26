/*******************************************************************************
*  Filename:       bsp_i2c.h
*  Revised:        $Date: $
*  Revision:       $Revision: $
*
*  Description:    Layer added above TI-RTOS driver managing switching between
*                  I2C-buses as well as an API that is compatible with the
*                  DriverLib only version.
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
#ifndef BSP_I2C_H
#define BSP_I2C_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "stdbool.h"
#include "stdint.h"
#include <inc/hw_memmap.h>

/*********************************************************************
 * CONSTANTS
 */
#define BSP_I2C_INTERFACE_0     0
#define BSP_I2C_INTERFACE_1     1
#define BSP_I2C_INTERFACE_NONE  -1

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * FUNCTIONS
 */
void bspI2cInit(void);
bool bspI2cSelect(uint8_t interface, uint8_t slaveAddress);
bool bspI2cRead(uint8_t *data, uint8_t len);
bool bspI2cWrite(uint8_t *data, uint8_t len);
bool bspI2cWriteSingle(uint8_t data);
bool bspI2cWriteRead(uint8_t *wdata, uint8_t wlen, uint8_t *rdata, uint8_t rlen);
void bspI2cDeselect(void);
void bspI2cDisable(void);
void bspI2cReset(void);

/////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif /* BSP_I2C_H */
