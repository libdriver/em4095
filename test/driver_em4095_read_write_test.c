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
 * @file      driver_em4095_read_write_test.c
 * @brief     driver em4095 read write test source file
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

#include "driver_em4095_read_write_test.h"
#include <stdlib.h>

static em4095_handle_t gs_handle;        /**< em4095 handle */
static volatile uint8_t gs_flag;            /**< flag */

/**
 * @brief  read_write test irq
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t em4095_read_write_test_irq_handler(void)
{
    if (em4095_irq_handler(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     interface receive callback
 * @param[in] mode is the running mode
 * @param[in] *buf points to a decode buffer
 * @param[in] len is the data length
 * @note      none
 */
static void a_receive_callback(em4095_mode_t mode, em4095_decode_t *buf, uint16_t len)
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
            gs_flag = 1;
            
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
            gs_flag = 1;
            
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
 * @brief     read write test
 * @param[in] times is the test times
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t em4095_read_write_test(uint32_t times)
{
    uint8_t res;
    uint16_t timeout;
    uint32_t i;
    uint32_t j;
    uint8_t buf[32];
    em4095_info_t info;
    
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
    
    /* get information */
    res = em4095_info(&info);
    if (res != 0)
    {
        em4095_interface_debug_print("em4095: get info failed.\n");
       
        return 1;
    }
    else
    {
        /* print chip info */
        em4095_interface_debug_print("em4095: chip is %s.\n", info.chip_name);
        em4095_interface_debug_print("em4095: manufacturer is %s.\n", info.manufacturer_name);
        em4095_interface_debug_print("em4095: interface is %s.\n", info.interface);
        em4095_interface_debug_print("em4095: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        em4095_interface_debug_print("em4095: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        em4095_interface_debug_print("em4095: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        em4095_interface_debug_print("em4095: max current is %0.2fmA.\n", info.max_current_ma);
        em4095_interface_debug_print("em4095: max temperature is %0.1fC.\n", info.temperature_max);
        em4095_interface_debug_print("em4095: min temperature is %0.1fC.\n", info.temperature_min);
    }
    
    /* init */
    res = em4095_init(&gs_handle);
    if (res != 0)
    {
        em4095_interface_debug_print("em4095: init failed.\n");
       
        return 1;
    }
    
    /* start read write test */
    em4095_interface_debug_print("em4095: start read write test.\n");
    
    /* set div */
    res = em4095_set_div(&gs_handle, 64);
    if (res != 0)
    {
        em4095_interface_debug_print("em4095: set div failed.\n");
        (void)em4095_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read test */
    em4095_interface_debug_print("em4095: read test.\n");
    
    /* loop */
    for (i = 0; i < times; i++)
    {
        /* 5s timeout */
        timeout = 500;
        
        /* init 0 */
        gs_flag = 0;
        
        /* read 128 bits */
        res = em4095_read(&gs_handle, 128);
        if (res != 0)
        {
            em4095_interface_debug_print("em4095: read failed.\n");
            (void)em4095_deinit(&gs_handle);
            
            return 1;
        }

        /* power on */
        res = em4095_power_on(&gs_handle);
        if (res != 0)
        {
            em4095_interface_debug_print("em4095: power on failed.\n");
            (void)em4095_deinit(&gs_handle);
            
            return 1;
        }
        
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
            /* receive timeout */
            em4095_interface_debug_print("em4095: read timeout.\n");
            (void)em4095_deinit(&gs_handle);
            
            return 1;
        }
    }
    
    /* write test */
    em4095_interface_debug_print("em4095: write test.\n");
    
    /* loop */
    for (i = 0; i < times; i++)
    {
    
        /* output */
        em4095_interface_debug_print("write buffer: ");
        
        /* make buffer */
        for (j = 0; j < 32; j++)
        {
            buf[j] = rand() % 2;
            em4095_interface_debug_print("0x%02X ", buf[j]);
        }
        
        /* output */
        em4095_interface_debug_print("\n");
        
        /* 5s timeout */
        timeout = 500;
        
        /* init 0 */
        gs_flag = 0;
        
        /* write 32 bits */
        res = em4095_write(&gs_handle, buf, 32);
        if (res != 0)
        {
            em4095_interface_debug_print("em4095: write failed.\n");
            (void)em4095_deinit(&gs_handle);
            
            return 1;
        }

        /* power on */
        res = em4095_power_on(&gs_handle);
        if (res != 0)
        {
            em4095_interface_debug_print("em4095: power on failed.\n");
            (void)em4095_deinit(&gs_handle);
            
            return 1;
        }
        
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
            /* receive timeout */
            em4095_interface_debug_print("em4095: write timeout.\n");
            (void)em4095_deinit(&gs_handle);
                
            return 1;
        }
    }
    
    /* finish read write test */
    em4095_interface_debug_print("em4095: finish read write test.\n");
    (void)em4095_deinit(&gs_handle);
    
    return 0;
}
