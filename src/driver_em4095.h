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
 * @file      driver_em4095.h
 * @brief     driver em4095 header file
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

#ifndef DRIVER_EM4095_H
#define DRIVER_EM4095_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup em4095_driver em4095 driver function
 * @brief    em4095 driver modules
 * @{
 */

/**
 * @addtogroup em4095_basic_driver
 * @{
 */

/**
 * @brief em4095 max length definition
 */
#ifndef EM4095_MAX_LENGTH
    #define EM4095_MAX_LENGTH   384        /**< 384 */
#endif

/**
 * @brief em4095 mode enumeration definition
 */
typedef enum
{
    EM4095_MODE_IDLE  = 0x00,        /**< idle */
    EM4095_MODE_READ  = 0x01,        /**< read */
    EM4095_MODE_WRITE = 0x02,        /**< write */
} em4095_mode_t;

/**
 * @brief em4095 time structure definition
 */
typedef struct em4095_time_s
{
    uint64_t s;         /**< second */
    uint32_t us;        /**< microsecond */
} em4095_time_t;

/**
 * @brief em4095 decode structure definition
 */
typedef struct em4095_decode_s
{
    em4095_time_t t;         /**< timestamp */
    uint32_t diff_us;        /**< diff us */
    uint8_t level;           /**< gpio level */
} em4095_decode_t;

/**
 * @brief em4095 handle structure definition
 */
typedef struct em4095_handle_s
{
    uint8_t (*shd_gpio_init)(void);                              /**< point to an shd_gpio_init function address */
    uint8_t (*shd_gpio_deinit)(void);                            /**< point to an shd_gpio_deinit function address */
    uint8_t (*shd_gpio_write)(uint8_t data);                     /**< point to an shd_gpio_write function address */
    uint8_t (*mod_gpio_init)(void);                              /**< point to an mod_gpio_init function address */
    uint8_t (*mod_gpio_deinit)(void);                            /**< point to an mod_gpio_deinit function address */
    uint8_t (*mod_gpio_write)(uint8_t data);                     /**< point to an mod_gpio_write function address */
    uint8_t (*demod_gpio_init)(void);                            /**< point to an demod_gpio_init function address */
    uint8_t (*demod_gpio_deinit)(void);                          /**< point to an demod_gpio_deinit function address */
    uint8_t (*demod_gpio_read)(uint8_t *data);                   /**< point to an demod_gpio_read function address */
    uint8_t (*timestamp_read)(em4095_time_t *t);                 /**< point to a timestamp_read function address */
    void (*delay_ms)(uint32_t ms);                               /**< point to a delay_ms function address */
    void (*debug_print)(const char *const fmt, ...);             /**< point to a debug_print function address */
    void (*receive_callback)(em4095_mode_t mode, 
                             em4095_decode_t *buf, 
                             uint16_t len);                      /**< point to a receive_callback function address */
    uint8_t inited;                                              /**< inited flag */
    em4095_decode_t decode[EM4095_MAX_LENGTH];                   /**< decode buffer */
    uint16_t decode_len;                                         /**< decode length */
    em4095_time_t last_time;                                     /**< last time */
    uint8_t mode;                                                /**< mode */
    uint16_t len;                                                /**< length */
    uint32_t div;                                                /**< div */
    uint32_t div_len;                                            /**< div length */
    uint8_t start_flag;                                          /**< start flag */
    uint8_t last_bit;                                            /**< last bit */
} em4095_handle_t;

/**
 * @brief em4095 information structure definition
 */
typedef struct em4095_info_s
{
    char chip_name[32];                /**< chip name */
    char manufacturer_name[32];        /**< manufacturer name */
    char interface[8];                 /**< chip interface name */
    float supply_voltage_min_v;        /**< chip min supply voltage */
    float supply_voltage_max_v;        /**< chip max supply voltage */
    float max_current_ma;              /**< chip max current */
    float temperature_min;             /**< chip min operating temperature */
    float temperature_max;             /**< chip max operating temperature */
    uint32_t driver_version;           /**< driver version */
} em4095_info_t;

/**
 * @}
 */

/**
 * @defgroup em4095_link_driver em4095 link driver function
 * @brief    em4095 link driver modules
 * @ingroup  em4095_driver
 * @{
 */

/**
 * @brief     initialize em4095_handle_t structure
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] STRUCTURE is em4095_handle_t
 * @note      none
 */
#define DRIVER_EM4095_LINK_INIT(HANDLE, STRUCTURE)           memset(HANDLE, 0, sizeof(STRUCTURE))

/**
 * @brief     link shd_gpio_init function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a shd_gpio_init function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_SHD_GPIO_INIT(HANDLE, FUC)       (HANDLE)->shd_gpio_init = FUC

/**
 * @brief     link shd_gpio_deinit function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a shd_gpio_deinit function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_SHD_GPIO_DEINIT(HANDLE, FUC)     (HANDLE)->shd_gpio_deinit = FUC

/**
 * @brief     link shd_gpio_write function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a shd_gpio_write function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_SHD_GPIO_WRITE(HANDLE, FUC)      (HANDLE)->shd_gpio_write = FUC

/**
 * @brief     link mod_gpio_init function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a mod_gpio_init function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_MOD_GPIO_INIT(HANDLE, FUC)       (HANDLE)->mod_gpio_init = FUC

/**
 * @brief     link mod_gpio_deinit function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a mod_gpio_deinit function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_MOD_GPIO_DEINIT(HANDLE, FUC)     (HANDLE)->mod_gpio_deinit = FUC

/**
 * @brief     link mod_gpio_write function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a mod_gpio_write function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_MOD_GPIO_WRITE(HANDLE, FUC)      (HANDLE)->mod_gpio_write = FUC

/**
 * @brief     link demod_gpio_init function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a demod_gpio_init function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_DEMOD_GPIO_INIT(HANDLE, FUC)     (HANDLE)->demod_gpio_init = FUC

/**
 * @brief     link demod_gpio_deinit function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a demod_gpio_deinit function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_DEMOD_GPIO_DEINIT(HANDLE, FUC)   (HANDLE)->demod_gpio_deinit = FUC

/**
 * @brief     link demod_gpio_read function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a demod_gpio_read function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_DEMOD_GPIO_READ(HANDLE, FUC)     (HANDLE)->demod_gpio_read = FUC

/**
 * @brief     link timestamp_read function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a timestamp_read function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_TIMESTAMP_READ(HANDLE, FUC)      (HANDLE)->timestamp_read = FUC

/**
 * @brief     link delay_ms function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a delay_ms function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_DELAY_MS(HANDLE, FUC)            (HANDLE)->delay_ms = FUC

/**
 * @brief     link debug_print function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a debug_print function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_DEBUG_PRINT(HANDLE, FUC)         (HANDLE)->debug_print = FUC

/**
 * @brief     link receive_callback function
 * @param[in] HANDLE points to an em4095 handle structure
 * @param[in] FUC points to a receive_callback function address
 * @note      none
 */
#define DRIVER_EM4095_LINK_RECEIVE_CALLBACK(HANDLE, FUC)    (HANDLE)->receive_callback = FUC

/**
 * @}
 */
 
/**
 * @defgroup em4095_basic_driver em4095 basic driver function
 * @brief    em4095 basic driver modules
 * @ingroup  em4095_driver
 * @{
 */

/**
 * @brief      get chip's information
 * @param[out] *info points to an em4095 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t em4095_info(em4095_info_t *info);

/**
 * @brief     irq handler
 * @param[in] *handle points to an em4095 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t em4095_irq_handler(em4095_handle_t *handle);

/**
 * @brief     initialize the chip
 * @param[in] *handle points to an em4095 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 gpio initialization failed
 *            - 2 handle is NULL
 *            - 3 linked functions is NULL
 * @note      none
 */
uint8_t em4095_init(em4095_handle_t *handle);

/**
 * @brief     close the chip
 * @param[in] *handle points to an em4095 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t em4095_deinit(em4095_handle_t *handle);

/**
 * @brief     read data
 * @param[in] *handle points to an em4095 handle structure
 * @param[in] len is the read length
 * @return    status code
 *            - 0 success
 *            - 1 read failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 len is too long
 * @note      none
 */
uint8_t em4095_read(em4095_handle_t *handle, uint16_t len);

/**
 * @brief     write data
 * @param[in] *handle points to an em4095 handle structure
 * @param[in] *buf points to a data buffer
 * @param[in] len is the write length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 len is too long
 * @note      none
 */
uint8_t em4095_write(em4095_handle_t *handle, uint8_t *buf, uint16_t len);

/**
 * @brief      copy decode buffer
 * @param[in]  *handle points to an em4095 handle structure
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the read length
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t em4095_copy_decode_buffer(em4095_handle_t *handle, uint8_t *buf, uint16_t *len);

/**
 * @brief     power on
 * @param[in] *handle points to an em4095 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 power on failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t em4095_power_on(em4095_handle_t *handle);

/**
 * @brief     power down
 * @param[in] *handle points to an em4095 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 power down failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t em4095_power_down(em4095_handle_t *handle);

/**
 * @brief     set irq div
 * @param[in] *handle points to an em4095 handle structure
 * @param[in] clock_div is the set div
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 div can't be 0
 * @note      none
 */
uint8_t em4095_set_div(em4095_handle_t *handle, uint32_t clock_div);

/**
 * @brief      get irq div
 * @param[in]  *handle points to an em4095 handle structure
 * @param[out] *clock_div points to a div buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t em4095_get_div(em4095_handle_t *handle, uint32_t *clock_div);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
