/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_em4095_interface.h
 * @brief     driver em4095 interface header file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2024-06-30
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2024/06/30  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#ifndef DRIVER_EM4095_INTERFACE_H
#define DRIVER_EM4095_INTERFACE_H

#include "driver_em4095.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup em4095_interface_driver em4095 interface driver function
 * @brief    em4095 interface driver modules
 * @ingroup  em4095_driver
 * @{
 */

/**
 * @brief  interface shd gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t em4095_interface_shd_gpio_init(void);

/**
 * @brief  interface shd gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t em4095_interface_shd_gpio_deinit(void);

/**
 * @brief     interface shd gpio write
 * @param[in] data is the set level
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t em4095_interface_shd_gpio_write(uint8_t data);

/**
 * @brief  interface mod gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t em4095_interface_mod_gpio_init(void);

/**
 * @brief  interface mod gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t em4095_interface_mod_gpio_deinit(void);

/**
 * @brief     interface mod gpio write
 * @param[in] data is the set level
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t em4095_interface_mod_gpio_write(uint8_t data);

/**
 * @brief  interface demod gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t em4095_interface_demod_gpio_init(void);

/**
 * @brief  interface demod gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t em4095_interface_demod_gpio_deinit(void);

/**
 * @brief      interface demod gpio read
 * @param[out] *data points to a data level buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t em4095_interface_demod_gpio_read(uint8_t *data);

/**
 * @brief     interface timestamp read
 * @param[in] *t points to an em4095_time structure
 * @return    status code
 *            - 0 success
 *            - 1 read failed
 * @note      none
 */
uint8_t em4095_interface_timestamp_read(em4095_time_t *t);

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void em4095_interface_delay_ms(uint32_t ms);

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void em4095_interface_debug_print(const char *const fmt, ...);

/**
 * @brief     interface receive callback
 * @param[in] mode is the running mode
 * @param[in] *buf points to a decode buffer
 * @param[in] len is the data length
 * @note      none
 */
void em4095_interface_receive_callback(em4095_mode_t mode, em4095_decode_t *buf, uint16_t len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
