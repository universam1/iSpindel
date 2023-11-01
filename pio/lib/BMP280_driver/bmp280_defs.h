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
* @file	bmp280_defs.h
* @date	2020-01-10
* @version	v3.3.4
*
*/#ifndef __BMP280_DEFS_H__
#define __BMP280_DEFS_H__

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************/
/*! @name       Header includes             */
/****************************************************************/
#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <stddef.h>
#endif

/****************************************************************/
/*! @name       Common macros               */
/****************************************************************/
#ifdef __KERNEL__
#if !defined(UINT8_C) && !defined(INT8_C)
#define INT8_C(x)   S8_C(x)
#define UINT8_C(x)  U8_C(x)
#endif

#if !defined(UINT16_C) && !defined(INT16_C)
#define INT16_C(x)  S16_C(x)
#define UINT16_C(x) U16_C(x)
#endif

#if !defined(INT32_C) && !defined(UINT32_C)
#define INT32_C(x)  S32_C(x)
#define UINT32_C(x) U32_C(x)
#endif

#if !defined(INT64_C) && !defined(UINT64_C)
#define INT64_C(x)  S64_C(x)
#define UINT64_C(x) U64_C(x)
#endif
#endif

/*! @name C standard macros */
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *) 0)
#endif
#endif

/****************************************************************/
/*! @name       BMP280 Macros               */
/****************************************************************/

/*! @name Macro to disable double precision floating point compensation
 * @note Uncomment the following line to disable it
 */
#ifndef BMP280_DISABLE_DOUBLE_COMPENSATION

/* #define BMP280_DISABLE_DOUBLE_COMPENSATION */
#endif

/*! @name Macro to disable 64bit compensation
 * @note Uncomment the following line to disable it
 */
#ifndef BMP280_DISABLE_64BIT_COMPENSATION

/* #define BMP280_DISABLE_64BIT_COMPENSATION */
#endif

/*! @name Interface selection macros */
#define BMP280_SPI_INTF                      UINT8_C(0)
#define BMP280_I2C_INTF                      UINT8_C(1)

/*! @name Return codes */
/*! @name Success code*/
#define BMP280_OK                            INT8_C(0)
#define BMP280_BOND_WIRE_OK                  INT8_C(0)

/*! @name Error codes */
#define BMP280_E_NULL_PTR                    INT8_C(-1)
#define BMP280_E_DEV_NOT_FOUND               INT8_C(-2)
#define BMP280_E_INVALID_LEN                 INT8_C(-3)
#define BMP280_E_COMM_FAIL                   INT8_C(-4)
#define BMP280_E_INVALID_MODE                INT8_C(-5)
#define BMP280_E_BOND_WIRE                   INT8_C(-6)
#define BMP280_E_IMPLAUS_TEMP                INT8_C(-7)
#define BMP280_E_IMPLAUS_PRESS               INT8_C(-8)
#define BMP280_E_CAL_PARAM_RANGE             INT8_C(-9)
#define BMP280_E_UNCOMP_TEMP_RANGE           INT8_C(-10)
#define BMP280_E_UNCOMP_PRES_RANGE           INT8_C(-11)
#define BMP280_E_UNCOMP_TEMP_AND_PRESS_RANGE INT8_C(-12)
#define BMP280_E_UNCOMP_DATA_CALC            INT8_C(-13)
#define BMP280_E_32BIT_COMP_TEMP             INT8_C(-14)
#define BMP280_E_32BIT_COMP_PRESS            INT8_C(-15)
#define BMP280_E_64BIT_COMP_PRESS            INT8_C(-16)
#define BMP280_E_DOUBLE_COMP_TEMP            INT8_C(-17)
#define BMP280_E_DOUBLE_COMP_PRESS           INT8_C(-18)

/*! @name Chip IDs for samples and mass production parts */
#define BMP280_CHIP_ID1                      UINT8_C(0x56)
#define BMP280_CHIP_ID2                      UINT8_C(0x57)
#define BMP280_CHIP_ID3                      UINT8_C(0x58)

/*! @name I2C addresses */
#define BMP280_I2C_ADDR_PRIM                 UINT8_C(0x76)
#define BMP280_I2C_ADDR_SEC                  UINT8_C(0x77)

/*! @name Calibration parameter register addresses*/
#define BMP280_DIG_T1_LSB_ADDR               UINT8_C(0x88)
#define BMP280_DIG_T1_MSB_ADDR               UINT8_C(0x89)
#define BMP280_DIG_T2_LSB_ADDR               UINT8_C(0x8A)
#define BMP280_DIG_T2_MSB_ADDR               UINT8_C(0x8B)
#define BMP280_DIG_T3_LSB_ADDR               UINT8_C(0x8C)
#define BMP280_DIG_T3_MSB_ADDR               UINT8_C(0x8D)
#define BMP280_DIG_P1_LSB_ADDR               UINT8_C(0x8E)
#define BMP280_DIG_P1_MSB_ADDR               UINT8_C(0x8F)
#define BMP280_DIG_P2_LSB_ADDR               UINT8_C(0x90)
#define BMP280_DIG_P2_MSB_ADDR               UINT8_C(0x91)
#define BMP280_DIG_P3_LSB_ADDR               UINT8_C(0x92)
#define BMP280_DIG_P3_MSB_ADDR               UINT8_C(0x93)
#define BMP280_DIG_P4_LSB_ADDR               UINT8_C(0x94)
#define BMP280_DIG_P4_MSB_ADDR               UINT8_C(0x95)
#define BMP280_DIG_P5_LSB_ADDR               UINT8_C(0x96)
#define BMP280_DIG_P5_MSB_ADDR               UINT8_C(0x97)
#define BMP280_DIG_P6_LSB_ADDR               UINT8_C(0x98)
#define BMP280_DIG_P6_MSB_ADDR               UINT8_C(0x99)
#define BMP280_DIG_P7_LSB_ADDR               UINT8_C(0x9A)
#define BMP280_DIG_P7_MSB_ADDR               UINT8_C(0x9B)
#define BMP280_DIG_P8_LSB_ADDR               UINT8_C(0x9C)
#define BMP280_DIG_P8_MSB_ADDR               UINT8_C(0x9D)
#define BMP280_DIG_P9_LSB_ADDR               UINT8_C(0x9E)
#define BMP280_DIG_P9_MSB_ADDR               UINT8_C(0x9F)

/*! @name Other registers */
#define BMP280_CHIP_ID_ADDR                  UINT8_C(0xD0)
#define BMP280_SOFT_RESET_ADDR               UINT8_C(0xE0)
#define BMP280_STATUS_ADDR                   UINT8_C(0xF3)
#define BMP280_CTRL_MEAS_ADDR                UINT8_C(0xF4)
#define BMP280_CONFIG_ADDR                   UINT8_C(0xF5)
#define BMP280_PRES_MSB_ADDR                 UINT8_C(0xF7)
#define BMP280_PRES_LSB_ADDR                 UINT8_C(0xF8)
#define BMP280_PRES_XLSB_ADDR                UINT8_C(0xF9)
#define BMP280_TEMP_MSB_ADDR                 UINT8_C(0xFA)
#define BMP280_TEMP_LSB_ADDR                 UINT8_C(0xFB)
#define BMP280_TEMP_XLSB_ADDR                UINT8_C(0xFC)

/*! @name Power modes */
#define BMP280_SLEEP_MODE                    UINT8_C(0x00)
#define BMP280_FORCED_MODE                   UINT8_C(0x01)
#define BMP280_NORMAL_MODE                   UINT8_C(0x03)

/*! @name Soft reset command */
#define BMP280_SOFT_RESET_CMD                UINT8_C(0xB6)

/*! @name ODR options */
#define BMP280_ODR_0_5_MS                    UINT8_C(0x00)
#define BMP280_ODR_62_5_MS                   UINT8_C(0x01)
#define BMP280_ODR_125_MS                    UINT8_C(0x02)
#define BMP280_ODR_250_MS                    UINT8_C(0x03)
#define BMP280_ODR_500_MS                    UINT8_C(0x04)
#define BMP280_ODR_1000_MS                   UINT8_C(0x05)
#define BMP280_ODR_2000_MS                   UINT8_C(0x06)
#define BMP280_ODR_4000_MS                   UINT8_C(0x07)

/*! @name Over-sampling macros */
#define BMP280_OS_NONE                       UINT8_C(0x00)
#define BMP280_OS_1X                         UINT8_C(0x01)
#define BMP280_OS_2X                         UINT8_C(0x02)
#define BMP280_OS_4X                         UINT8_C(0x03)
#define BMP280_OS_8X                         UINT8_C(0x04)
#define BMP280_OS_16X                        UINT8_C(0x05)

/*! @name Filter coefficient macros */
#define BMP280_FILTER_OFF                    UINT8_C(0x00)
#define BMP280_FILTER_COEFF_2                UINT8_C(0x01)
#define BMP280_FILTER_COEFF_4                UINT8_C(0x02)
#define BMP280_FILTER_COEFF_8                UINT8_C(0x03)
#define BMP280_FILTER_COEFF_16               UINT8_C(0x04)

/*! @name SPI 3-Wire macros */
#define BMP280_SPI3_WIRE_ENABLE              UINT8_C(1)
#define BMP280_SPI3_WIRE_DISABLE             UINT8_C(0)

/*! @name Measurement status */
#define BMP280_MEAS_DONE                     UINT8_C(0)
#define BMP280_MEAS_ONGOING                  UINT8_C(1)

/*! @name Image update */
#define BMP280_IM_UPDATE_DONE                UINT8_C(0)
#define BMP280_IM_UPDATE_ONGOING             UINT8_C(1)

/*! @name Position and mask macros */
#define BMP280_STATUS_IM_UPDATE_POS          UINT8_C(0)
#define BMP280_STATUS_IM_UPDATE_MASK         UINT8_C(0x01)
#define BMP280_STATUS_MEAS_POS               UINT8_C(3)
#define BMP280_STATUS_MEAS_MASK              UINT8_C(0x08)
#define BMP280_OS_TEMP_POS                   UINT8_C(5)
#define BMP280_OS_TEMP_MASK                  UINT8_C(0xE0)
#define BMP280_OS_PRES_POS                   UINT8_C(2)
#define BMP280_OS_PRES_MASK                  UINT8_C(0x1C)
#define BMP280_POWER_MODE_POS                UINT8_C(0)
#define BMP280_POWER_MODE_MASK               UINT8_C(0x03)
#define BMP280_STANDBY_DURN_POS              UINT8_C(5)
#define BMP280_STANDBY_DURN_MASK             UINT8_C(0xE0)
#define BMP280_FILTER_POS                    UINT8_C(2)
#define BMP280_FILTER_MASK                   UINT8_C(0x1C)
#define BMP280_SPI3_ENABLE_POS               UINT8_C(0)
#define BMP280_SPI3_ENABLE_MASK              UINT8_C(0x01)

/*! @name Calibration parameters' relative position */
#define BMP280_DIG_T1_LSB_POS                UINT8_C(0)
#define BMP280_DIG_T1_MSB_POS                UINT8_C(1)
#define BMP280_DIG_T2_LSB_POS                UINT8_C(2)
#define BMP280_DIG_T2_MSB_POS                UINT8_C(3)
#define BMP280_DIG_T3_LSB_POS                UINT8_C(4)
#define BMP280_DIG_T3_MSB_POS                UINT8_C(5)
#define BMP280_DIG_P1_LSB_POS                UINT8_C(6)
#define BMP280_DIG_P1_MSB_POS                UINT8_C(7)
#define BMP280_DIG_P2_LSB_POS                UINT8_C(8)
#define BMP280_DIG_P2_MSB_POS                UINT8_C(9)
#define BMP280_DIG_P3_LSB_POS                UINT8_C(10)
#define BMP280_DIG_P3_MSB_POS                UINT8_C(11)
#define BMP280_DIG_P4_LSB_POS                UINT8_C(12)
#define BMP280_DIG_P4_MSB_POS                UINT8_C(13)
#define BMP280_DIG_P5_LSB_POS                UINT8_C(14)
#define BMP280_DIG_P5_MSB_POS                UINT8_C(15)
#define BMP280_DIG_P6_LSB_POS                UINT8_C(16)
#define BMP280_DIG_P6_MSB_POS                UINT8_C(17)
#define BMP280_DIG_P7_LSB_POS                UINT8_C(18)
#define BMP280_DIG_P7_MSB_POS                UINT8_C(19)
#define BMP280_DIG_P8_LSB_POS                UINT8_C(20)
#define BMP280_DIG_P8_MSB_POS                UINT8_C(21)
#define BMP280_DIG_P9_LSB_POS                UINT8_C(22)
#define BMP280_DIG_P9_MSB_POS                UINT8_C(23)
#define BMP280_CALIB_DATA_SIZE               UINT8_C(24)

/*! @name Bit-slicing macros */
#define BMP280_GET_BITS(bitname, x)                    ((x & bitname##_MASK) \
                                                        >> bitname##_POS)
#define BMP280_SET_BITS(regvar, bitname, val)          ((regvar & \
                                                         ~bitname##_MASK) | ((val << bitname##_POS) & bitname##_MASK))
#define BMP280_SET_BITS_POS_0(reg_data, bitname, data) ((reg_data & \
                                                         ~(bitname##_MASK)) | (data & bitname##_MASK))
#define BMP280_GET_BITS_POS_0(bitname, reg_data)       (reg_data & \
                                                        (bitname##_MASK))

/*! @brief Macros holding the minimum and maximum for trimming values */

#define BMP280_ST_DIG_T1_MIN UINT16_C(19000)
#define BMP280_ST_DIG_T1_MAX UINT16_C(35000)
#define BMP280_ST_DIG_T2_MIN UINT16_C(22000)
#define BMP280_ST_DIG_T2_MAX UINT16_C(30000)
#define BMP280_ST_DIG_T3_MIN INT16_C(-3000)
#define BMP280_ST_DIG_T3_MAX INT16_C(-1000)
#define BMP280_ST_DIG_P1_MIN UINT16_C(30000)
#define BMP280_ST_DIG_P1_MAX UINT16_C(42000)
#define BMP280_ST_DIG_P2_MIN INT16_C(-12970)
#define BMP280_ST_DIG_P2_MAX INT16_C(-8000)
#define BMP280_ST_DIG_P3_MIN INT16_C(-5000)
#define BMP280_ST_DIG_P3_MAX UINT16_C(8000)
#define BMP280_ST_DIG_P4_MIN INT16_C(-10000)
#define BMP280_ST_DIG_P4_MAX UINT16_C(18000)
#define BMP280_ST_DIG_P5_MIN INT16_C(-500)
#define BMP280_ST_DIG_P5_MAX UINT16_C(1100)
#define BMP280_ST_DIG_P6_MIN INT16_C(-1000)
#define BMP280_ST_DIG_P6_MAX UINT16_C(1000)
#define BMP280_ST_DIG_P7_MIN INT16_C(-32768)
#define BMP280_ST_DIG_P7_MAX UINT16_C(32767)
#define BMP280_ST_DIG_P8_MIN INT16_C(-30000)
#define BMP280_ST_DIG_P8_MAX UINT16_C(10000)
#define BMP280_ST_DIG_P9_MIN INT16_C(-10000)
#define BMP280_ST_DIG_P9_MAX UINT16_C(30000)

#define BMP280_GET_BITSLICE(regvar, bitname) \
    ((regvar & bitname##__MSK) >> bitname##__POS)

/*! @brief Macros to read out API revision number */
/*Register holding custom trimming values */
#define BMP280_ST_TRIMCUSTOM_REG             UINT8_C(0x87)
#define BMP280_ST_TRIMCUSTOM_REG_APIREV__POS UINT8_C(1)
#define BMP280_ST_TRIMCUSTOM_REG_APIREV__MSK UINT8_C(0x06)
#define BMP280_ST_TRIMCUSTOM_REG_APIREV__LEN UINT8_C(2)
#define BMP280_ST_TRIMCUSTOM_REG_APIREV__REG BMP280_ST_TRIMCUSTOM_REG

/* highest API revision supported is revision 0. */
#define BMP280_ST_MAX_APIREVISION            UINT8_C(0x00)

/*! @brief Macros holding the minimum and maximum for trimming values */
/* 0x00000 is minimum output value */
#define BMP280_ST_ADC_T_MIN                  INT32_C(0x00000)

/* 0xFFFF0 is maximum 20-bit output value without over sampling */
#define BMP280_ST_ADC_T_MAX                  INT32_C(0xFFFF0)

/* 0x00000 is minimum output value */
#define BMP280_ST_ADC_P_MIN                  INT32_C(0x00000)

/* 0xFFFF0 is maximum 20-bit output value without over sampling */
#define BMP280_ST_ADC_P_MAX                  INT32_C(0xFFFF0)

/*! @name Function pointer type definitions */
typedef int8_t (*bmp280_com_fptr_t)(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);
typedef void (*bmp280_delay_fptr_t)(uint32_t period);

/*! @name Calibration parameters' structure */
struct bmp280_calib_param
{
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;
    int32_t t_fine;
};

/*! @name Sensor configuration structure */
struct bmp280_config
{
    uint8_t os_temp;
    uint8_t os_pres;
    uint8_t odr;
    uint8_t filter;
    uint8_t spi3w_en;
};

/*! @name Sensor status structure */
struct bmp280_status
{
    uint8_t measuring;
    uint8_t im_update;
};

/*! @name Uncompensated data structure */
struct bmp280_uncomp_data
{
    int32_t uncomp_temp;
    uint32_t uncomp_press;
};

/*! @name API device structure */
struct bmp280_dev
{
    uint8_t chip_id;
    uint8_t dev_id;
    uint8_t intf;
    bmp280_com_fptr_t read;
    bmp280_com_fptr_t write;
    bmp280_delay_fptr_t delay_ms;
    struct bmp280_calib_param calib_param;
    struct bmp280_config conf;
};

#ifdef __cplusplus
}
#endif /* End of CPP guard */

#endif /* __BMP280_DEFS_H__ */
