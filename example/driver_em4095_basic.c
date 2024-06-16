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
 * @file      driver_em4095_basic.c
 * @brief     driver em4095 basic source file
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

#include "driver_em4095_basic.h"

static em4095_handle_t gs_handle;                                                                 /**< em4095 handle */
static volatile uint8_t gs_flag;                                                                  /**< flag */
static void (*gs_callback)(em4095_mode_t mode, em4095_decode_t *buf, uint16_t len) = NULL;        /**< callback */

/**
 * @brief     interface receive callback
 * @param[in] mode is the running mode
 * @param[in] *buf points to a decode buffer
 * @param[in] len is the data length
 * @note      none
 */
static void a_receive_callback(em4095_mode_t mode, em4095_decode_t *buf, uint16_t len)
{
    switch (mode)
    {
        case EM4095_MODE_READ :
        {
            gs_flag = 1;
            if (gs_callback != NULL)
            {
                gs_callback(mode, buf, len);
            }

            break;
        }
        case EM4095_MODE_WRITE :
        {
            gs_flag = 1;
            if (gs_callback != NULL)
            {
                gs_callback(mode, buf, len);
            }

            break;
        }
        default :
        {
            em4095_interface_debug_print("em4095: irq unknown mode.\n");

            break;
        }
    }
}

/**
 * @brief  basic irq
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t em4095_basic_irq_handler(void)
{
    if (em4095_irq_handler(&gs_handle) != 0)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief     basic example init
 * @param[in] *callback points to an irq callback address
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t em4095_basic_init(void (*callback)(em4095_mode_t mode, em4095_decode_t *buf, uint16_t len))
{
    uint8_t res;

    /* link interface function */
    DRIVER_EM4095_LINK_INIT(&gs_handle, em4095_handle_t);
    DRIVER_EM4095_LINK_SHD_GPIO_INIT(&gs_handle, em4095_interface_shd_gpio_init);
    DRIVER_EM4095_LINK_SHD_GPIO_DEINIT(&gs_handle, em4095_interface_shd_gpio_deinit);
    DRIVER_EM4095_LINK_SHD_GPIO_WRITE(&gs_handle, em4095_interface_shd_gpio_write);
    DRIVER_EM4095_LINK_MOD_GPIO_INIT(&gs_handle, em4095_interface_mod_gpio_init);
    DRIVER_EM4095_LINK_MOD_GPIO_DEINIT(&gs_handle, em4095_interface_mod_gpio_deinit);
    DRIVER_EM4095_LINK_MOD_GPIO_WRITE(&gs_handle, em4095_interface_mod_gpio_write);
    DRIVER_EM4095_LINK_DEMOD_GPIO_INIT(&gs_handle, em4095_interface_demod_gpio_init);
    DRIVER_EM4095_LINK_DEMOD_GPIO_DEINIT(&gs_handle, em4095_interface_demod_gpio_deinit);
    DRIVER_EM4095_LINK_DEMOD_GPIO_READ(&gs_handle, em4095_interface_demod_gpio_read);
    DRIVER_EM4095_LINK_TIMESTAMP_READ(&gs_handle, em4095_interface_timestamp_read);
    DRIVER_EM4095_LINK_DELAY_MS(&gs_handle, em4095_interface_delay_ms);
    DRIVER_EM4095_LINK_DEBUG_PRINT(&gs_handle, em4095_interface_debug_print);
    DRIVER_EM4095_LINK_RECEIVE_CALLBACK(&gs_handle, a_receive_callback);
    gs_callback = callback;

    /* init */
    res = em4095_init(&gs_handle);
    if (res != 0)
    {
        em4095_interface_debug_print("em4095: init failed.\n");

        return 1;
    }

    /* set default div */
    res = em4095_set_div(&gs_handle, EM4095_BASIC_DEFAULT_DIV);
    if (res != 0)
    {
        em4095_interface_debug_print("em4095: set div failed.\n");
        (void)em4095_deinit(&gs_handle);

        return 1;
    }

    /* power down */
    res = em4095_power_down(&gs_handle);
    if (res != 0)
    {
        em4095_interface_debug_print("em4095: power down failed.\n");
        (void)em4095_deinit(&gs_handle);

        return 1;
    }

    return 0;
}

/**
 * @brief     basic example set clock div
 * @param[in] clock_div is the set clock div
 * @return    status code
 *            - 0 success
 *            - 1 set div failed
 * @note      none
 */
uint8_t em4095_basic_set_div(uint32_t clock_div)
{
    uint8_t res;
    
    /* set div */
    res = em4095_set_div(&gs_handle, clock_div);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      basic example read
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the read length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t em4095_basic_read(uint8_t *buf, uint16_t len)
{
    uint8_t res;
    uint16_t l;
    uint32_t timeout;

    /* init 0 */
    gs_flag = 0;

    /* read */
    res = em4095_read(&gs_handle, len);
    if (res != 0)
    {
        return 1;
    }

    /* power on */
    res = em4095_power_on(&gs_handle);
    if (res != 0)
    {
        return 1;
    }

    /* set timeout 5s */
    timeout = 500;

    /* check timeout */
    while (timeout != 0)
    {
        /* check the flag */
        if (gs_flag != 0)
        {
            break;
        }

        /* timeout -- */
        timeout--;

        /* delay 10ms */
        em4095_interface_delay_ms(10);
    }

    /* check the timeout */
    if (timeout == 0)
    {
        return 1;
    }

    /* copy to buffer */
    res = em4095_copy_decode_buffer(&gs_handle, buf, &l);
    if (res != 0)
    {
        return 1;
    }

    /* check length */
    if (l != len)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief     basic example write
 * @param[in] *buf points to a data buffer
 * @param[in] len is the write length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t em4095_basic_write(uint8_t *buf, uint16_t len)
{
    uint8_t res;
    uint32_t timeout;

    /* init 0 */
    gs_flag = 0;

    /* write */
    res = em4095_write(&gs_handle, buf, len);
    if (res != 0)
    {
        return 1;
    }

    /* power on */
    res = em4095_power_on(&gs_handle);
    if (res != 0)
    {
        return 1;
    }

    /* set timeout 5s */
    timeout = 500;

    /* check timeout */
    while (timeout != 0)
    {
        /* check the flag */
        if (gs_flag != 0)
        {
            break;
        }

        /* timeout -- */
        timeout--;

        /* delay 10ms */
        em4095_interface_delay_ms(10);
    }

    /* check the timeout */
    if (timeout == 0)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t em4095_basic_deinit(void)
{
    if (em4095_deinit(&gs_handle) != 0)
    {
        return 1;
    }
    gs_callback = NULL;

    return 0;
}
