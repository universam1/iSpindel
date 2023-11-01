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
* @file	bmp280.h
* @date	2020-01-10
* @version	v3.3.4
*
*/#ifndef __BMP280_H__
#define __BMP280_H__

#include "bmp280_defs.h"

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief This API reads the data from the given register address of the
 * sensor.
 *
 * @param[in] reg_addr : Register address from where the data to be read
 * @param[out] reg_data : Pointer to data buffer to store the read data.
 * @param[in] len : No of bytes of data to be read.
 * @param[in] dev : Structure instance of bmp280_dev.
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint8_t len, const struct bmp280_dev *dev);

/*!
 * @brief This API writes the given data to the register addresses
 * of the sensor.
 *
 * @param[in] reg_addr : Register address from where the data to be written.
 * @param[in] reg_data : Pointer to data buffer which is to be written
 * in the sensor.
 * @param[in] len : No of bytes of data to write..
 * @param[in] dev : Structure instance of bmp280_dev.
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_set_regs(uint8_t *reg_addr, const uint8_t *reg_data, uint8_t len, const struct bmp280_dev *dev);

/*!
 * @brief This API triggers the soft reset of the sensor.
 *
 * @param[in] dev : Structure instance of bmp280_dev.
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise..
 */
int8_t bmp280_soft_reset(const struct bmp280_dev *dev);

/*!
 *  @brief This API is the entry point.
 *  It reads the chip-id and calibration data from the sensor.
 *
 *  @param[in,out] dev : Structure instance of bmp280_dev
 *
 *  @return Result of API execution
 *  @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
int8_t bmp280_init(struct bmp280_dev *dev);

/*!
 * @brief This API reads the data from the ctrl_meas register and config
 * register. It gives the currently set temperature and pressure over-sampling
 * configuration, power mode configuration, sleep duration and
 * IIR filter coefficient.
 *
 * @param[out] conf : Current configuration of the bmp280
 * conf.osrs_t, conf.osrs_p = BMP280_OS_NONE, BMP280_OS_1X,
 *     BMP280_OS_2X, BMP280_OS_4X, BMP280_OS_8X, BMP280_OS_16X
 * conf.odr = BMP280_ODR_0_5_MS, BMP280_ODR_62_5_MS, BMP280_ODR_125_MS,
 *     BMP280_ODR_250_MS, BMP280_ODR_500_MS, BMP280_ODR_1000_MS,
 *     BMP280_ODR_2000_MS, BMP280_ODR_4000_MS
 * conf.filter = BMP280_FILTER_OFF, BMP280_FILTER_COEFF_2,
 *     BMP280_FILTER_COEFF_4, BMP280_FILTER_COEFF_8, BMP280_FILTER_COEFF_16
 * conf.spi3w_en = BMP280_SPI3_WIRE_ENABLE, BMP280_SPI3_WIRE_DISABLE
 * @param[in] dev : Structure instance of bmp280_dev
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_get_config(struct bmp280_config *conf, struct bmp280_dev *dev);

/*!
 * @brief This API writes the data to the ctrl_meas register and config register.
 * It sets the temperature and pressure over-sampling configuration,
 * power mode configuration, sleep duration and IIR filter coefficient.
 *
 * @param[in] conf : Desired configuration to the bmp280
 * conf.osrs_t, conf.osrs_p = BMP280_OS_NONE, BMP280_OS_1X,
 *     BMP280_OS_2X, BMP280_OS_4X, BMP280_OS_8X, BMP280_OS_16X
 * conf.odr = BMP280_ODR_0_5_MS, BMP280_ODR_62_5_MS, BMP280_ODR_125_MS,
 *     BMP280_ODR_250_MS, BMP280_ODR_500_MS, BMP280_ODR_1000_MS,
 *     BMP280_ODR_2000_MS, BMP280_ODR_4000_MS
 * conf.filter = BMP280_FILTER_OFF, BMP280_FILTER_COEFF_2,
 *     BMP280_FILTER_COEFF_4, BMP280_FILTER_COEFF_8, BMP280_FILTER_COEFF_16
 * conf.spi3w_en = BMP280_SPI3_WIRE_ENABLE, BMP280_SPI3_WIRE_DISABLE
 * @param[in] dev : Structure instance of bmp280_dev
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_set_config(const struct bmp280_config *conf, struct bmp280_dev *dev);

/*!
 * @brief This API reads the status register
 *
 * @param[out] status : Status of the sensor
 * status.measuring = BMP280_MEAS_DONE, BMP280_MEAS_ONGOING
 * status.im_update = BMP280_IM_UPDATE_DONE, BMP280_IM_UPDATE_ONGOING
 * @param[in] dev : structure instance of bmp280_dev
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_get_status(struct bmp280_status *status, const struct bmp280_dev *dev);

/*!
 * @brief This API reads the power mode.
 *
 * @param[out] mode : BMP280_SLEEP_MODE, BMP280_NORMAL_MODE,
 *     BMP280_FORCED_MODE
 * @param[in] dev : Structure instance of bmp280_dev
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_get_power_mode(uint8_t *mode, const struct bmp280_dev *dev);

/*!
 * @brief This API writes the power mode.
 *
 * @param[out] mode : BMP280_SLEEP_MODE, BMP280_NORMAL_MODE,
 *     BMP280_FORCED_MODE
 * @param[in] dev : Structure instance of bmp280_dev
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_set_power_mode(uint8_t mode, struct bmp280_dev *dev);

/*!
 * @brief This API reads the temperature and pressure data registers.
 * It gives the raw temperature and pressure data.
 *
 * @param[in] uncomp_data : Structure instance of bmp280_uncomp_data
 * @param[in] dev : Structure instance of bmp280_dev
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_get_uncomp_data(struct bmp280_uncomp_data *uncomp_data, const struct bmp280_dev *dev);

/*!
 * @brief This API is used to get the compensated temperature from
 * uncompensated temperature. This API uses 32 bit integers.
 * Temperature in degC, resolution is 0.01 DegC. output value of
 * "5123" equals 51.23 degree Celsius
 *
 * @param[out] comp_temp : 32 bit compensated temperature
 * @param[in] uncomp_temp : Raw temperature values from the sensor
 * @param[in] dev : Structure instance of bmp280_dev
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_get_comp_temp_32bit(int32_t *comp_temp, int32_t uncomp_temp, struct bmp280_dev *dev);

/*!
 * @brief This API is used to get the compensated pressure from
 * uncompensated pressure. This API uses 32 bit integers.
 * Pressure in Pa as unsigned 32 bit integer
 * output value of "96386" equals 96386 Pa = 963.86 hPa
 *
 * @param[out] comp_pres : 32 bit compensated pressure
 * @param[in] uncomp_pres : Raw pressure values from the sensor
 * @param[in] dev : structure instance of bmp280_dev
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_get_comp_pres_32bit(uint32_t *comp_pres, uint32_t uncomp_pres, const struct bmp280_dev *dev);

#ifndef BMP280_DISABLE_64BIT_COMPENSATION

/*!
 * @brief This API is used to get the compensated pressure from
 * uncompensated pressure. This API uses 64 bit integers.
 * Pressure in Pa as unsigned 32 bit integer in Q24.8 format
 * (24 integer bits and 8 fractional bits). Output value of "24674867"
 * represents 24674867/256 = 96386.2 Pa = 963.862 hPa
 *
 * @param[out] pressure : compensated pressure
 * @param[in] uncomp_pres : Raw pressure values from the sensor
 * @param[in] dev : Structure instance of bmp280_dev
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_get_comp_pres_64bit(uint32_t *pressure, uint32_t uncomp_pres, const struct bmp280_dev *dev);

#endif /* BMP280_DISABLE_64BIT_COMPENSATION */

#ifndef BMP280_DISABLE_DOUBLE_COMPENSATION

/*!
 * @brief This API is used to get the compensated temperature from
 * uncompensated temperature. This API uses double floating precision.
 * Temperature in degree Celsius , double precision. output value
 * of "51.23" equals 51.23 degC.
 *
 * @param[out] temperature : compensated temperature
 * @param[in] uncomp_temp : Raw temperature values from the sensor
 * @param[in] dev : Structure instance of bmp280_dev
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_get_comp_temp_double(double *temperature, int32_t uncomp_temp, struct bmp280_dev *dev);

/*!
 * @brief This API is used to get the compensated pressure from
 * uncompensated pressure. This API uses double floating precision.
 * Pressure in Pa as double. Output value of "96386.2"
 * equals 96386.2 Pa = 963.862 hPa
 *
 * @param[out] pressure : compensated pressure
 * @param[in] uncomp_pres : Raw pressure values from the sensor
 * @param[in] dev : Structure instance of bmp280_dev
 *
 * @return Result of API execution
 * @retval Zero for Success, non-zero otherwise.
 */
int8_t bmp280_get_comp_pres_double(double *pressure, uint32_t uncomp_pres, const struct bmp280_dev *dev);

#endif /* BMP280_DISABLE_DOUBLE_COMPENSATION */

/*!
 * @brief This API computes the measurement time in milliseconds for the
 * active configuration
 *
 * @param[in] dev : Structure instance of bmp280_dev
 *
 * @return Measurement time for the active configuration in milliseconds
 */
uint8_t bmp280_compute_meas_time(const struct bmp280_dev *dev);

#ifdef __cplusplus
}
#endif /* End of CPP guard */

#endif /* _BMP280_H_ */
