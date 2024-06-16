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
 * @file      stm32f407_driver_em4095_interface.c
 * @brief     stm32f407 driver em4095 interface source file
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

#include "driver_em4095_interface.h"
#include "delay.h"
#include "wire.h"
#include "uart.h"
#include "tim.h"
#include <stdarg.h>

/**
 * @brief timer var definition
 */
static uint64_t gs_second = 0;        /**< second */

/**
 * @brief     timer callback
 * @param[in] us is the timer cnt
 * @note      none
 */
static void gs_tim_irq(uint32_t us)
{
    gs_second += us / 1000000;
}

/**
 * @brief  interface timer init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t em4095_interface_timer_init(void)
{
    /* timer init */
    if (tim_init(1000000, gs_tim_irq) != 0)
    {
        return 1;
    }
    
    /* timer start */
    if (tim_start() != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  interface shd gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t em4095_interface_shd_gpio_init(void)
{
    return wire_en_init();
}

/**
 * @brief  interface shd gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t em4095_interface_shd_gpio_deinit(void)
{
    return wire_en_deinit();
}

/**
 * @brief     interface shd gpio write
 * @param[in] data is the set level
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t em4095_interface_shd_gpio_write(uint8_t data)
{
    return wire_en_write(data);
}

/**
 * @brief  interface mod gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t em4095_interface_mod_gpio_init(void)
{
    return wire_ce_init();
}

/**
 * @brief  interface mod gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t em4095_interface_mod_gpio_deinit(void)
{
    return wire_ce_deinit();
}

/**
 * @brief     interface mod gpio write
 * @param[in] data is the set level
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t em4095_interface_mod_gpio_write(uint8_t data)
{
    return wire_ce_write(data);
}

/**
 * @brief  interface demod gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t em4095_interface_demod_gpio_init(void)
{
    return wire_init();
}

/**
 * @brief  interface demod gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t em4095_interface_demod_gpio_deinit(void)
{
    return wire_deinit();
}

/**
 * @brief      interface demod gpio read
 * @param[out] *data points to a data level buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t em4095_interface_demod_gpio_read(uint8_t *data)
{
    return wire_read(data);
}

/**
 * @brief     interface timestamp read
 * @param[in] *t points to an em4095_time structure
 * @return    status code
 *            - 0 success
 *            - 1 read failed
 * @note      none
 */
uint8_t em4095_interface_timestamp_read(em4095_time_t *t)
{
    t->s = gs_second;
    t->us = tim_get_handle()->Instance->CNT;
    
    return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void em4095_interface_delay_ms(uint32_t ms)
{
    delay_ms(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void em4095_interface_debug_print(const char *const fmt, ...)
{
    char str[256];
    uint16_t len;
    va_list args;
    
    memset((char *)str, 0, sizeof(char) * 256); 
    va_start(args, fmt);
    vsnprintf((char *)str, 255, (char const *)fmt, args);
    va_end(args);
    
    len = strlen((char *)str);
    (void)uart_write((uint8_t *)str, len);
}

/**
 * @brief     interface receive callback
 * @param[in] mode is the running mode
 * @param[in] *buf points to a decode buffer
 * @param[in] len is the data length
 * @note      none
 */
void em4095_interface_receive_callback(em4095_mode_t mode, em4095_decode_t *buf, uint16_t len)
{
    uint16_t i;
    
    switch (mode)
    {
        case EM4095_MODE_READ :
        {
            em4095_interface_debug_print("em4095: irq read done.\n");
            em4095_interface_debug_print("raw data: ");
            for (i = 0; i < len; i++)
            {
                em4095_interface_debug_print("0x%02X ", buf[i].level);
            }
            em4095_interface_debug_print(".\n");
            
            break;
        }
        case EM4095_MODE_WRITE :
        {
            em4095_interface_debug_print("em4095: irq write done.\n");
            em4095_interface_debug_print("raw data: ");
            for (i = 0; i < len; i++)
            {
                em4095_interface_debug_print("0x%02X ", buf[i].level);
            }
            em4095_interface_debug_print(".\n");
            
            break;
        }
        default :
        {
            em4095_interface_debug_print("em4095: irq unknown mode.\n");
            
            break;
        }
    }
}
