#ifndef __CAP_TOUCH_H__
#define __CAP_TOUCH_H__

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "stdio.h"

#define FT6X06_I2C_ADDR                 0x38

//*****************************************************************************
// Fill out the #defines below to configure which pins are connected to
// the I2C Bus
//*****************************************************************************
#define FT6X06_FOCALTECH_ID_R         0xA8

#include "cyhal_i2c.h"
#include <stdbool.h>
#include <stdint.h>
#include "cy_result.h"

/* Read/write one byte via I2C */
cy_rslt_t cap_touch_read_u8(cyhal_i2c_t *obj, uint8_t reg, uint8_t *value);
cy_rslt_t cap_touch_write_u8(cyhal_i2c_t *obj, uint8_t reg, uint8_t value);

/* Returns number of active touch points (0 = not touching) */
uint8_t cap_touch_get_num_touches(cyhal_i2c_t *obj);

/* Reads X and Y coordinates of first touch point */
bool cap_touch_get_xy(cyhal_i2c_t *obj, uint16_t *x, uint16_t *y);

#endif

