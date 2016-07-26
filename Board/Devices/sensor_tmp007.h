/******************************************************************************
*  Filename:       sensor_tmp007.h
*  Revised:        $Date: 2014-07-30 15:20:17 +0200 (on, 30 jul 2014) $
*  Revision:       $Revision: 13441 $
*
*  Description:    Interface to the IR temperature sensor driver TI TMP007.
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
******************************************************************************/
#ifndef SENSOR_TMP007_H
#define SENSOR_TMP007_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "stdint.h"

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * FUNCTIONS
 */
bool sensorTmp007Init(void);
bool sensorTmp007Enable(bool enable);
bool sensorTmp007Test(void);

bool sensorTmp006Read(uint16_t *rawVolt, uint16_t *rawTemp);
void sensorTmp006Convert(uint16_t rawVolt, uint16_t rawTemp, float *tObj, float *tTgt);

bool sensorTmp007Read(uint16_t *rawTemp, uint16_t *rawObjTemp);
void sensorTmp007Convert(uint16_t rawTemp, uint16_t rawObjTemp, float *tObj, float *tTgt);


#ifdef __cplusplus
}
#endif

#endif /* SENSOR_TMP007_H */
