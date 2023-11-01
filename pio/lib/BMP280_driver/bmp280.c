/**
* Copyright (c) 2020 Bosch Sensortec GmbH. All rights reserved.
*
* BSD-3-Clause
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* @file	bmp280.c
* @date	2020-01-10
* @version	v3.3.4
*
*/#include "bmp280.h"

/********************** Static function declarations ************************/

/*!
 * @brief This internal API is used to check for null-pointers in the device
 * structure.
 *
 * @param[in] dev : Structure instance of bmp280_dev.
 *
 * @return Result of API execution status
 * @retval Zero for Success, non-zero otherwise.
 */
static int8_t null_ptr_check(const struct bmp280_dev *dev);

/*!
 * @brief This internal API interleaves the register addresses and respective
 * register data for a burst write
 *
 * @param[in] reg_addr: Register address array
 * @param[out] temp_buff: Interleaved register address and data array
 * @param[in] reg_addr: Register data array
 * @param[in] len : Length of the reg_addr and reg_data arrays
 *
 * @return Result of API execution status
 * @retval Zero for Success, non-zero otherwise.
 */
static void interleave_data(const uint8_t *reg_addr, uint8_t *temp_buff, const uint8_t *reg_data, uint8_t len);

/*!
 * @brief This API is used to read the calibration parameters used
 * for calculating the compensated data.
 *
 * @param[in] dev : Structure instance of bmp280_dev
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
static int8_t get_calib_param(struct bmp280_dev *dev);

/*!
 * @brief This internal API to reset the sensor, restore/set conf, restore/set mode
 *
 * @param[in] mode: Desired mode
 * @param[in] conf : Desired configuration to the bmp280
 * conf.os_temp, conf.os_pres = BMP280_OS_NONE, BMP280_OS_1X,
 *     BMP280_OS_2X, BMP280_OS_4X, BMP280_OS_8X, BMP280_OS_16X
 * conf.mode = BMP280_SLEEP_MODE, BMP280_FORCED_MODE, BMP280_NORMAL_MODE
 * conf.odr = BMP280_ODR_0_5_MS, BMP280_ODR_62_5_MS, BMP280_ODR_125_MS,
 *     BMP280_ODR_250_MS, BMP280_ODR_500_MS, BMP280_ODR_1000_MS,
 *     BMP280_ODR_2000_MS, BMP280_ODR_4000_MS
 * conf.filter = BMP280_FILTER_OFF, BMP280_FILTER_COEFF_2,
 *     BMP280_FILTER_COEFF_4, BMP280_FILTER_COEFF_8, BMP280_FILTER_COEFF_16
 * @param[in] dev : Structure instance of bmp280_dev
 *
 * @return Result of API execution status
 * @retval Zero for Success, non-zero otherwise.
 */
static int8_t conf_sensor(uint8_t mode, const struct bmp280_config *conf, struct bmp280_dev *dev);

/*!
 * @This internal API checks whether the uncompensated temperature and pressure are within the range
 *
 * @param[in] utemperature : uncompensated temperature
 * @param[in] upressure : uncompensated pressure
 *
 * @return Result of API execution status
 * @retval Zero for Success, non-zero otherwise.
 */
static int8_t st_check_boundaries(int32_t utemperature, int32_t upressure);

/****************** User Function Definitions *******************************/

/*!
 * @brief This API reads the data from the given register address of the
 * sensor.
 */
int8_t bmp280_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint8_t len, const struct bmp280_dev *dev)
{
    int8_t rslt;

    rslt = null_ptr_check(dev);
    if ((rslt == BMP280_OK) && (reg_data != NULL))
    {
        /* Mask the register address' MSB if interface selected is SPI */
        if (dev->intf == BMP280_SPI_INTF)
        {
            reg_addr = reg_addr | 0x80;
        }
        rslt = dev->read(dev->dev_id, reg_addr, reg_data, len);

        /* Check for communication error and mask with an internal error code */
        if (rslt != BMP280_OK)
        {
            rslt = BMP280_E_COMM_FAIL;
        }
    }
    else
    {
        rslt = BMP280_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API writes the given data to the register addresses
 * of the sensor.
 */
int8_t bmp280_set_regs(uint8_t *reg_addr, const uint8_t *reg_data, uint8_t len, const struct bmp280_dev *dev)
{
    int8_t rslt;
    uint8_t temp_buff[8]; /* Typically not to write more than 4 registers */
    uint16_t temp_len;
    uint8_t reg_addr_cnt;

    if (len > 4)
    {
        len = 4;
    }
    rslt = null_ptr_check(dev);
    if ((rslt == BMP280_OK) && (reg_addr != NULL) && (reg_data != NULL))
    {
        if (len != 0)
        {
            temp_buff[0] = reg_data[0];

            /* Mask the register address' MSB if interface selected is SPI */
            if (dev->intf == BMP280_SPI_INTF)
            {
                /* Converting all the reg address into proper SPI write address
                 * i.e making MSB(R/`W) bit 0
                 */
                for (reg_addr_cnt = 0; reg_addr_cnt < len; reg_addr_cnt++)
                {
                    reg_addr[reg_addr_cnt] = reg_addr[reg_addr_cnt] & 0x7F;
                }
            }

            /* Burst write mode */
            if (len > 1)
            {
                /* Interleave register address w.r.t data for burst write*/
                interleave_data(reg_addr, temp_buff, reg_data, len);
                temp_len = ((len * 2) - 1);
            }
            else
            {
                temp_len = len;
            }
            rslt = dev->write(dev->dev_id, reg_addr[0], temp_buff, temp_len);

            /* Check for communication error and mask with an internal error code */
            if (rslt != BMP280_OK)
            {
                rslt = BMP280_E_COMM_FAIL;
            }
        }
        else
        {
            rslt = BMP280_E_INVALID_LEN;
        }
    }
    else
    {
        rslt = BMP280_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API triggers the soft reset of the sensor.
 */
int8_t bmp280_soft_reset(const struct bmp280_dev *dev)
{
    int8_t rslt;
    uint8_t reg_addr = BMP280_SOFT_RESET_ADDR;
    uint8_t soft_rst_cmd = BMP280_SOFT_RESET_CMD;

    rslt = null_ptr_check(dev);
    if (rslt == BMP280_OK)
    {
        rslt = bmp280_set_regs(&reg_addr, &soft_rst_cmd, 1, dev);

        /* As per the datasheet, startup time is 2 ms. */
        dev->delay_ms(2);
    }

    return rslt;
}

/*!
 * @brief This API is the entry point.
 * It reads the chip-id and calibration data from the sensor.
 */
int8_t bmp280_init(struct bmp280_dev *dev)
{
    int8_t rslt;

    /* Maximum number of tries before timeout */
    uint8_t try_count = 5;

    rslt = null_ptr_check(dev);
    if (rslt == BMP280_OK)
    {
        while (try_count)
        {
            rslt = bmp280_get_regs(BMP280_CHIP_ID_ADDR, &dev->chip_id, 1, dev);

            /* Check for chip id validity */
            if ((rslt == BMP280_OK) &&
                (dev->chip_id == BMP280_CHIP_ID1 || dev->chip_id == BMP280_CHIP_ID2 || dev->chip_id == BMP280_CHIP_ID3))
            {
                rslt = bmp280_soft_reset(dev);
                if (rslt == BMP280_OK)
                {
                    rslt = get_calib_param(dev);
                }
                break;
            }

            /* Wait for 10 ms */
            dev->delay_ms(10);
            --try_count;
        }

        /* Chip id check failed, and timed out */
        if (!try_count)
        {
            rslt = BMP280_E_DEV_NOT_FOUND;
        }
        if (rslt == BMP280_OK)
        {
            /* Set values to default */
            dev->conf.filter = BMP280_FILTER_OFF;
            dev->conf.os_pres = BMP280_OS_NONE;
            dev->conf.os_temp = BMP280_OS_NONE;
            dev->conf.odr = BMP280_ODR_0_5_MS;
            dev->conf.spi3w_en = BMP280_SPI3_WIRE_DISABLE;
        }
    }

    return rslt;
}

/*!
 * @brief This API reads the data from the ctrl_meas register and config
 * register. It gives the currently set temperature and pressure over-sampling
 * configuration, power mode configuration, sleep duration and
 * IIR filter coefficient.
 */
int8_t bmp280_get_config(struct bmp280_config *conf, struct bmp280_dev *dev)
{
    int8_t rslt;
    uint8_t temp[2] = { 0, 0 };

    rslt = null_ptr_check(dev);
    if ((rslt == BMP280_OK) && (conf != NULL))
    {
        rslt = bmp280_get_regs(BMP280_CTRL_MEAS_ADDR, temp, 2, dev);
        if (rslt == BMP280_OK)
        {
            conf->os_temp = BMP280_GET_BITS(BMP280_OS_TEMP, temp[0]);
            conf->os_pres = BMP280_GET_BITS(BMP280_OS_PRES, temp[0]);
            conf->odr = BMP280_GET_BITS(BMP280_STANDBY_DURN, temp[1]);
            conf->filter = BMP280_GET_BITS(BMP280_FILTER, temp[1]);
            conf->spi3w_en = BMP280_GET_BITS_POS_0(BMP280_SPI3_ENABLE, temp[1]);
            dev->conf = *conf;
        }
    }
    else
    {
        rslt = BMP280_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API writes the data to the ctrl_meas register and config register.
 * It sets the temperature and pressure over-sampling configuration,
 * power mode configuration, sleep duration and IIR filter coefficient.
 */
int8_t bmp280_set_config(const struct bmp280_config *conf, struct bmp280_dev *dev)
{
    return conf_sensor(BMP280_SLEEP_MODE, conf, dev);
}

/*!
 * @brief This API reads the status register
 */
int8_t bmp280_get_status(struct bmp280_status *status, const struct bmp280_dev *dev)
{
    int8_t rslt;
    uint8_t temp;

    rslt = null_ptr_check(dev);
    if ((rslt == BMP280_OK) && (status != NULL))
    {
        rslt = bmp280_get_regs(BMP280_STATUS_ADDR, &temp, 1, dev);
        status->measuring = BMP280_GET_BITS(BMP280_STATUS_MEAS, temp);
        status->im_update = BMP280_GET_BITS_POS_0(BMP280_STATUS_IM_UPDATE, temp);
    }
    else
    {
        rslt = BMP280_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API reads the power mode.
 */
int8_t bmp280_get_power_mode(uint8_t *mode, const struct bmp280_dev *dev)
{
    int8_t rslt;
    uint8_t temp;

    rslt = null_ptr_check(dev);
    if ((rslt == BMP280_OK) && (mode != NULL))
    {
        rslt = bmp280_get_regs(BMP280_CTRL_MEAS_ADDR, &temp, 1, dev);
        *mode = BMP280_GET_BITS_POS_0(BMP280_POWER_MODE, temp);
    }
    else
    {
        rslt = BMP280_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API writes the power mode.
 */
int8_t bmp280_set_power_mode(uint8_t mode, struct bmp280_dev *dev)
{
    int8_t rslt;

    rslt = null_ptr_check(dev);
    if (rslt == BMP280_OK)
    {
        rslt = conf_sensor(mode, &dev->conf, dev);
    }

    return rslt;
}

/*!
 * @brief This API reads the temperature and pressure data registers.
 * It gives the raw temperature and pressure data .
 */
int8_t bmp280_get_uncomp_data(struct bmp280_uncomp_data *uncomp_data, const struct bmp280_dev *dev)
{
    int8_t rslt;
    uint8_t temp[6] = { 0 };

    rslt = null_ptr_check(dev);
    if ((rslt == BMP280_OK) && (uncomp_data != NULL))
    {
        rslt = bmp280_get_regs(BMP280_PRES_MSB_ADDR, temp, 6, dev);
        if (rslt == BMP280_OK)
        {
            uncomp_data->uncomp_press =
                (int32_t) ((((uint32_t) (temp[0])) << 12) | (((uint32_t) (temp[1])) << 4) | ((uint32_t) temp[2] >> 4));
            uncomp_data->uncomp_temp =
                (int32_t) ((((int32_t) (temp[3])) << 12) | (((int32_t) (temp[4])) << 4) | (((int32_t) (temp[5])) >> 4));
            rslt = st_check_boundaries((int32_t)uncomp_data->uncomp_temp, (int32_t)uncomp_data->uncomp_press);
        }
        else
        {
            rslt = BMP280_E_UNCOMP_DATA_CALC;
        }
    }
    else
    {
        rslt = BMP280_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API is used to get the compensated temperature from
 * uncompensated temperature. This API uses 32 bit integers.
 */
int8_t bmp280_get_comp_temp_32bit(int32_t *comp_temp, int32_t uncomp_temp, struct bmp280_dev *dev)
{
    int32_t var1, var2;
    int8_t rslt;

    rslt = null_ptr_check(dev);
    if (rslt == BMP280_OK)
    {
        var1 =
            ((((uncomp_temp / 8) - ((int32_t) dev->calib_param.dig_t1 << 1))) * ((int32_t) dev->calib_param.dig_t2)) /
            2048;
        var2 =
            (((((uncomp_temp / 16) - ((int32_t) dev->calib_param.dig_t1)) *
               ((uncomp_temp / 16) - ((int32_t) dev->calib_param.dig_t1))) / 4096) *
             ((int32_t) dev->calib_param.dig_t3)) /
            16384;
        dev->calib_param.t_fine = var1 + var2;
        *comp_temp = (dev->calib_param.t_fine * 5 + 128) / 256;
        rslt = BMP280_OK;
    }
    else
    {
        *comp_temp = 0;
        rslt = BMP280_E_32BIT_COMP_TEMP;
    }

    return rslt;
}

/*!
 * @brief This API is used to get the compensated pressure from
 * uncompensated pressure. This API uses 32 bit integers.
 */
int8_t bmp280_get_comp_pres_32bit(uint32_t *comp_pres, uint32_t uncomp_pres, const struct bmp280_dev *dev)
{
    int32_t var1, var2;
    int8_t rslt;

    rslt = null_ptr_check(dev);
    if (rslt == BMP280_OK)
    {
        var1 = (((int32_t) dev->calib_param.t_fine) / 2) - (int32_t) 64000;
        var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t) dev->calib_param.dig_p6);
        var2 = var2 + ((var1 * ((int32_t) dev->calib_param.dig_p5)) * 2);
        var2 = (var2 / 4) + (((int32_t) dev->calib_param.dig_p4) * 65536);
        var1 =
            (((dev->calib_param.dig_p3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8) +
             ((((int32_t) dev->calib_param.dig_p2) * var1) / 2)) / 262144;
        var1 = ((((32768 + var1)) * ((int32_t) dev->calib_param.dig_p1)) / 32768);
        *comp_pres = (uint32_t)(((int32_t)(1048576 - uncomp_pres) - (var2 / 4096)) * 3125);

        /* Avoid exception caused by division with zero */
        if (var1 != 0)
        {
            /* Check for overflows against UINT32_MAX/2; if pres is left-shifted by 1 */
            if (*comp_pres < 0x80000000)
            {
                *comp_pres = (*comp_pres << 1) / ((uint32_t) var1);
            }
            else
            {
                *comp_pres = (*comp_pres / (uint32_t) var1) * 2;
            }
            var1 = (((int32_t) dev->calib_param.dig_p9) * ((int32_t) (((*comp_pres / 8) * (*comp_pres / 8)) / 8192))) /
                   4096;
            var2 = (((int32_t) (*comp_pres / 4)) * ((int32_t) dev->calib_param.dig_p8)) / 8192;
            *comp_pres = (uint32_t) ((int32_t) *comp_pres + ((var1 + var2 + dev->calib_param.dig_p7) / 16));
            rslt = BMP280_OK;
        }
        else
        {
            *comp_pres = 0;
            rslt = BMP280_E_32BIT_COMP_PRESS;
        }
    }

    return rslt;
}

#ifndef BMP280_DISABLE_64BIT_COMPENSATION

/*!
 * @brief This API is used to get the compensated pressure from
 * uncompensated pressure. This API uses 64 bit integers.
 */
int8_t bmp280_get_comp_pres_64bit(uint32_t *pressure, uint32_t uncomp_pres, const struct bmp280_dev *dev)
{
    int64_t var1, var2, p;
    int8_t rslt;

    rslt = null_ptr_check(dev);
    if (rslt == BMP280_OK)
    {
        var1 = ((int64_t) (dev->calib_param.t_fine)) - 128000;
        var2 = var1 * var1 * (int64_t) dev->calib_param.dig_p6;
        var2 = var2 + ((var1 * (int64_t) dev->calib_param.dig_p5) * 131072);
        var2 = var2 + (((int64_t) dev->calib_param.dig_p4) * 34359738368);
        var1 = ((var1 * var1 * (int64_t) dev->calib_param.dig_p3) / 256) +
               ((var1 * (int64_t) dev->calib_param.dig_p2) * 4096);
        var1 = ((INT64_C(0x800000000000) + var1) * ((int64_t)dev->calib_param.dig_p1)) / 8589934592;
        if (var1 != 0)
        {
            p = 1048576 - uncomp_pres;
            p = (((((p * 2147483648U)) - var2) * 3125) / var1);
            var1 = (((int64_t) dev->calib_param.dig_p9) * (p / 8192) * (p / 8192)) / 33554432;
            var2 = (((int64_t) dev->calib_param.dig_p8) * p) / 524288;
            p = ((p + var1 + var2) / 256) + (((int64_t)dev->calib_param.dig_p7) * 16);
            *pressure = (uint32_t)p;
            rslt = BMP280_OK;
        }
        else
        {
            *pressure = 0;
            rslt = BMP280_E_64BIT_COMP_PRESS;
        }
    }

    return rslt;
}

#endif /* BMP280_DISABLE_64BIT_COMPENSATION */

#ifndef BMP280_DISABLE_DOUBLE_COMPENSATION

/*!
 * @brief This API is used to get the compensated temperature from
 * uncompensated temperature. This API uses double floating precision.
 */
int8_t bmp280_get_comp_temp_double(double *temperature, int32_t uncomp_temp, struct bmp280_dev *dev)
{
    double var1, var2;
    int8_t rslt;

    rslt = null_ptr_check(dev);
    if (rslt == BMP280_OK)
    {
        var1 = (((double) uncomp_temp) / 16384.0 - ((double) dev->calib_param.dig_t1) / 1024.0) *
               ((double) dev->calib_param.dig_t2);
        var2 =
            ((((double) uncomp_temp) / 131072.0 - ((double) dev->calib_param.dig_t1) / 8192.0) *
             (((double) uncomp_temp) / 131072.0 - ((double) dev->calib_param.dig_t1) / 8192.0)) *
            ((double) dev->calib_param.dig_t3);
        dev->calib_param.t_fine = (int32_t) (var1 + var2);
        *temperature = ((var1 + var2) / 5120.0);
    }
    else
    {
        *temperature = 0;
        rslt = BMP280_E_DOUBLE_COMP_TEMP;
    }

    return rslt;
}

/*!
 * @brief This API is used to get the compensated pressure from
 * uncompensated pressure. This API uses double floating precision.
 */
int8_t bmp280_get_comp_pres_double(double *pressure, uint32_t uncomp_pres, const struct bmp280_dev *dev)
{
    double var1, var2;
    int8_t rslt;

    rslt = null_ptr_check(dev);
    if (rslt == BMP280_OK)
    {
        var1 = ((double) dev->calib_param.t_fine / 2.0) - 64000.0;
        var2 = var1 * var1 * ((double) dev->calib_param.dig_p6) / 32768.0;
        var2 = var2 + var1 * ((double) dev->calib_param.dig_p5) * 2.0;
        var2 = (var2 / 4.0) + (((double) dev->calib_param.dig_p4) * 65536.0);
        var1 = (((double)dev->calib_param.dig_p3) * var1 * var1 / 524288.0 + ((double)dev->calib_param.dig_p2) * var1) /
               524288.0;
        var1 = (1.0 + var1 / 32768.0) * ((double) dev->calib_param.dig_p1);

        *pressure = 1048576.0 - (double)uncomp_pres;
        if (var1 < 0 || var1 > 0)
        {
            *pressure = (*pressure - (var2 / 4096.0)) * 6250.0 / var1;
            var1 = ((double)dev->calib_param.dig_p9) * (*pressure) * (*pressure) / 2147483648.0;
            var2 = (*pressure) * ((double)dev->calib_param.dig_p8) / 32768.0;
            *pressure = *pressure + (var1 + var2 + ((double)dev->calib_param.dig_p7)) / 16.0;
        }
        else
        {
            *pressure = 0;
            rslt = BMP280_E_DOUBLE_COMP_PRESS;
        }
    }

    return rslt;
}

#endif /* BMP280_DISABLE_DOUBLE_COMPENSATION */

/*!
 * @brief This API computes the measurement time in milliseconds for the
 * active configuration
 */
uint8_t bmp280_compute_meas_time(const struct bmp280_dev *dev)
{
    uint32_t period = 0;
    uint32_t t_dur = 0, p_dur = 0, p_startup = 0;
    const uint32_t startup = 1000, period_per_osrs = 2000; /* Typical timings in us. Maximum is +15% each */
    int8_t rslt;

    rslt = null_ptr_check(dev);
    if (rslt == BMP280_OK)
    {
        t_dur = period_per_osrs * ((UINT32_C(1) << dev->conf.os_temp) >> 1);
        p_dur = period_per_osrs * ((UINT32_C(1) << dev->conf.os_pres) >> 1);
        p_startup = (dev->conf.os_pres) ? 500 : 0;

        /* Increment the value to next highest integer if greater than 0.5 */
        period = startup + t_dur + p_startup + p_dur + 500;
        period /= 1000; /* Convert to milliseconds */
    }

    return (uint8_t)period;
}

/****************** Static Function Definitions *******************************/

/*!
 * @brief This internal API is used to check for null-pointers in the device
 * structure.
 */
static int8_t null_ptr_check(const struct bmp280_dev *dev)
{
    int8_t rslt;

    if ((dev == NULL) || (dev->read == NULL) || (dev->write == NULL) || (dev->delay_ms == NULL))
    {
        /* Null-pointer found */
        rslt = BMP280_E_NULL_PTR;
    }
    else
    {
        rslt = BMP280_OK;
    }

    return rslt;
}

/*!
 * @brief This internal API interleaves the register addresses and respective
 * register data for a burst write
 */
static void interleave_data(const uint8_t *reg_addr, uint8_t *temp_buff, const uint8_t *reg_data, uint8_t len)
{
    uint8_t index;

    for (index = 1; index < len; index++)
    {
        temp_buff[(index * 2) - 1] = reg_addr[index];
        temp_buff[index * 2] = reg_data[index];
    }
}

/*!
 * @brief This API is used to read the calibration parameters used
 * for calculating the compensated data.
 */
static int8_t get_calib_param(struct bmp280_dev *dev)
{
    int8_t rslt;
    uint8_t temp[BMP280_CALIB_DATA_SIZE] = { 0 };

    rslt = null_ptr_check(dev);
    if (rslt == BMP280_OK)
    {
        rslt = bmp280_get_regs(BMP280_DIG_T1_LSB_ADDR, temp, BMP280_CALIB_DATA_SIZE, dev);
        if (rslt == BMP280_OK)
        {
            dev->calib_param.dig_t1 =
                (uint16_t) (((uint16_t) temp[BMP280_DIG_T1_MSB_POS] << 8) | ((uint16_t) temp[BMP280_DIG_T1_LSB_POS]));
            dev->calib_param.dig_t2 =
                (int16_t) (((int16_t) temp[BMP280_DIG_T2_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_T2_LSB_POS]));
            dev->calib_param.dig_t3 =
                (int16_t) (((int16_t) temp[BMP280_DIG_T3_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_T3_LSB_POS]));
            dev->calib_param.dig_p1 =
                (uint16_t) (((uint16_t) temp[BMP280_DIG_P1_MSB_POS] << 8) | ((uint16_t) temp[BMP280_DIG_P1_LSB_POS]));
            dev->calib_param.dig_p2 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P2_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P2_LSB_POS]));
            dev->calib_param.dig_p3 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P3_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P3_LSB_POS]));
            dev->calib_param.dig_p4 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P4_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P4_LSB_POS]));
            dev->calib_param.dig_p5 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P5_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P5_LSB_POS]));
            dev->calib_param.dig_p6 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P6_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P6_LSB_POS]));
            dev->calib_param.dig_p7 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P7_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P7_LSB_POS]));
            dev->calib_param.dig_p8 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P8_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P8_LSB_POS]));
            dev->calib_param.dig_p9 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P9_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P9_LSB_POS]));
        }
    }

    return rslt;
}

/*!
 * @brief This internal API to reset the sensor, restore/set conf, restore/set mode
 */
static int8_t conf_sensor(uint8_t mode, const struct bmp280_config *conf, struct bmp280_dev *dev)
{
    int8_t rslt;
    uint8_t temp[2] = { 0, 0 };
    uint8_t reg_addr[2] = { BMP280_CTRL_MEAS_ADDR, BMP280_CONFIG_ADDR };

    rslt = null_ptr_check(dev);
    if ((rslt == BMP280_OK) && (conf != NULL))
    {
        rslt = bmp280_get_regs(BMP280_CTRL_MEAS_ADDR, temp, 2, dev);
        if (rslt == BMP280_OK)
        {
            /* Here the intention is to put the device to sleep
             * within the shortest period of time
             */
            rslt = bmp280_soft_reset(dev);
            if (rslt == BMP280_OK)
            {
                temp[0] = BMP280_SET_BITS(temp[0], BMP280_OS_TEMP, conf->os_temp);
                temp[0] = BMP280_SET_BITS(temp[0], BMP280_OS_PRES, conf->os_pres);
                temp[1] = BMP280_SET_BITS(temp[1], BMP280_STANDBY_DURN, conf->odr);
                temp[1] = BMP280_SET_BITS(temp[1], BMP280_FILTER, conf->filter);
                temp[1] = BMP280_SET_BITS_POS_0(temp[1], BMP280_SPI3_ENABLE, conf->spi3w_en);
                rslt = bmp280_set_regs(reg_addr, temp, 2, dev);
                if (rslt == BMP280_OK)
                {
                    dev->conf = *conf;
                    if (mode != BMP280_SLEEP_MODE)
                    {
                        /* Write only the power mode register in a separate write */
                        temp[0] = BMP280_SET_BITS_POS_0(temp[0], BMP280_POWER_MODE, mode);
                        rslt = bmp280_set_regs(reg_addr, temp, 1, dev);
                    }
                }
            }
        }
    }
    else
    {
        rslt = BMP280_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @This internal API checks whether the uncompensated temperature and pressure are within the range
 */
static int8_t st_check_boundaries(int32_t utemperature, int32_t upressure)
{
    int8_t rslt = 0;

    /* check UT and UP for valid range */
    if ((utemperature <= BMP280_ST_ADC_T_MIN || utemperature >= BMP280_ST_ADC_T_MAX) &&
        (upressure <= BMP280_ST_ADC_P_MIN || upressure >= BMP280_ST_ADC_P_MAX))
    {
        rslt = BMP280_E_UNCOMP_TEMP_AND_PRESS_RANGE;
    }
    else if (utemperature <= BMP280_ST_ADC_T_MIN || utemperature >= BMP280_ST_ADC_T_MAX)
    {
        rslt = BMP280_E_UNCOMP_TEMP_RANGE;
    }
    else if (upressure <= BMP280_ST_ADC_P_MIN || upressure >= BMP280_ST_ADC_P_MAX)
    {
        rslt = BMP280_E_UNCOMP_PRES_RANGE;
    }
    else
    {
        rslt = BMP280_OK;
    }

    return rslt;
}
