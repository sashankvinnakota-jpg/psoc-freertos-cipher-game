/**
 * @file cap_touch.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief Driver for the FT6236 capacitive touch controller
 * @version 0.1
 * @date 2026-03-24
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "cap_touch.h"
#include "i2c.h"

/* FT6236 Register Addresses (from datasheet page 26) */
#define FT6X06_REG_TD_STATUS    0x02    /* Number of touch points [3:0] */
#define FT6X06_REG_P1_XH        0x03    /* Touch 1 X high byte [3:0] + event flag [7:6] */
#define FT6X06_REG_P1_XL        0x04    /* Touch 1 X low byte */
#define FT6X06_REG_P1_YH        0x05    /* Touch 1 Y high byte [3:0] + touch ID [7:4] */
#define FT6X06_REG_P1_YL        0x06    /* Touch 1 Y low byte */

/**
 * @brief Read one byte from the FT6236 at the given register
 */
cy_rslt_t cap_touch_read_u8(cyhal_i2c_t *obj, uint8_t reg, uint8_t *value)
{
    return i2c_read_u8(obj, FT6X06_I2C_ADDR, reg, value);
}

/**
 * @brief Write one byte to the FT6236 at the given register
 */
cy_rslt_t cap_touch_write_u8(cyhal_i2c_t *obj, uint8_t reg, uint8_t value)
{
    return i2c_write_u8(obj, FT6X06_I2C_ADDR, reg, value);
}

/**
 * @brief Returns the number of active touch points (0 if not touching)
 */
uint8_t cap_touch_get_num_touches(cyhal_i2c_t *obj)
{
    uint8_t td_status = 0;
    cap_touch_read_u8(obj, FT6X06_REG_TD_STATUS, &td_status);

    /* Bits [3:0] hold the number of touch points */
    return (td_status & 0x0F);
}

/**
 * @brief Reads the X and Y position of the first touch point
 * @param obj    I2C object
 * @param x      Output: X coordinate
 * @param y      Output: Y coordinate
 * @return true if read succeeded, false otherwise
 */
bool cap_touch_get_xy(cyhal_i2c_t *obj, uint16_t *x, uint16_t *y)
{
    uint8_t xh = 0, xl = 0, yh = 0, yl = 0;
    cy_rslt_t rslt;

    rslt  = cap_touch_read_u8(obj, FT6X06_REG_P1_XH, &xh);
    rslt |= cap_touch_read_u8(obj, FT6X06_REG_P1_XL, &xl);
    rslt |= cap_touch_read_u8(obj, FT6X06_REG_P1_YH, &yh);
    rslt |= cap_touch_read_u8(obj, FT6X06_REG_P1_YL, &yl);

    if (rslt != CY_RSLT_SUCCESS)
    {
        return false;
    }

    /* Check event flag in bits [7:6] of XH.
     * 0x00 = Press Down  → valid, accept
     * 0x01 = Lift Up     → finger leaving, reject (stale coordinates)
     * 0x02 = Contact     → valid, accept (finger held down)
     * 0x03 = No Event    → invalid, reject */
    uint8_t event_flag = (xh >> 6) & 0x03;
    if (event_flag == 0x01 || event_flag == 0x03)
    {
        return false;
    }

    /* X is bits [3:0] of XH combined with all 8 bits of XL */
    *x = (uint16_t)((xh & 0x0F) << 8) | xl;

    /* Y is bits [3:0] of YH combined with all 8 bits of YL */
    *y = (uint16_t)((yh & 0x0F) << 8) | yl;

    return true;
}
