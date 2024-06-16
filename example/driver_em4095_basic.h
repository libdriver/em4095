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
 * @file      driver_em4095_basic.h
 * @brief     driver em4095 basic header file
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

#ifndef DRIVER_EM4095_BASIC_H
#define DRIVER_EM4095_BASIC_H

#include "driver_em4095_interface.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup em4095_example_driver em4095 example driver function
 * @brief    em4095 example driver modules
 * @ingroup  em4095_driver
 * @{
 */

/**
 * @brief em4095 basic example default definition
 */
#define EM4095_BASIC_DEFAULT_DIV     64       /**< div 64 */

/**
 * @brief  basic irq
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t em4095_basic_irq_handler(void);

/**
 * @brief     basic example init
 * @param[in] *callback points to an irq callback address
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t em4095_basic_init(void (*callback)(em4095_mode_t mode, em4095_decode_t *buf, uint16_t len));

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t em4095_basic_deinit(void);

/**
 * @brief      basic example read
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the read length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t em4095_basic_read(uint8_t *buf, uint16_t len);

/**
 * @brief     basic example write
 * @param[in] *buf points to a data buffer
 * @param[in] len is the write length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t em4095_basic_write(uint8_t *buf, uint16_t len);

/**
 * @brief     basic example set clock div
 * @param[in] clock_div is the set clock div
 * @return    status code
 *            - 0 success
 *            - 1 set div failed
 * @note      none
 */
uint8_t em4095_basic_set_div(uint32_t clock_div);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
