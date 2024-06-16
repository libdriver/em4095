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
 * @file      driver_em4095.c
 * @brief     driver em4095 source file
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

#include "driver_em4095.h"

/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "EM Microelectronic EM4095"        /**< chip name */
#define MANUFACTURER_NAME         "EM Microelectronic"               /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        4.1f                               /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        5.5f                               /**< chip max supply voltage */
#define MAX_CURRENT               300.0f                             /**< chip max current */
#define TEMPERATURE_MIN           -40.0f                             /**< chip min operating temperature */
#define TEMPERATURE_MAX           110.0f                             /**< chip max operating temperature */
#define DRIVER_VERSION            1000                               /**< driver version */

/**
 * @brief     em4095 get diff time
 * @param[in] *handle points to an em4095 handle structure
 * @note      none
 */
static void a_em4095_time_diff(em4095_handle_t *handle)
{
    uint16_t i;
    uint16_t len;

    len = handle->decode_len - 1;                                      /* len - 1 */
    for (i = 0; i < len; i++)                                          /* diff all time */
    {
        int64_t diff;

        diff = (int64_t)((int64_t)handle->decode[i + 1].t.s -
               (int64_t)handle->decode[i].t.s) * 1000000 +
               (int64_t)((int64_t)handle->decode[i + 1].t.us -
               (int64_t)handle->decode[i].t.us);                       /* diff time */
        handle->decode[i].diff_us = (uint32_t)diff;                    /* save the time diff */
    }
}

/**
 * @brief     read sync
 * @param[in] *handle points to an em4095 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 *            - 2 not sync
 *            - 3 not reach sync times
 * @note      none
 */
static uint8_t a_read_sync(em4095_handle_t *handle)
{
    uint8_t res;

    if (handle->start_flag < 1)                                              /* check start flag */
    {
        uint8_t level;

        res = handle->demod_gpio_read(&level);                               /* read gpio level */
        if (res != 0)                                                        /* check result */
        {
            handle->debug_print("em4095: demod gpio read failed.\n");        /* demod gpio read failed */

            return 1;                                                        /* return error */
        }
        if (handle->last_bit != level)                                       /* check last bit */
        {
            handle->last_bit = level;                                        /* save bit */
            handle->start_flag++;                                            /* start flag++ */
            if (handle->start_flag >= 1)                                     /* check flag */
            {
                goto start;                                                  /* goto flag */
            }

            return 2;                                                        /* return error */
        }
        else
        {
            return 3;                                                        /* return error */
        }
    }

    start:
    handle->div_len++;                                                       /* div length++ */
    if (handle->div_len >= handle->div)                                      /* wait for div */
    {
        handle->div_len = 0;                                                 /* init to 0 */

        return 0;                                                            /* success return 0 */
    }
    else
    {
        return 2;                                                            /* return error */
    }
}

/**
 * @brief     write sync
 * @param[in] *handle points to an em4095 handle structure
 * @return    status code
 *            - 0 success
 *            - 2 not sync
 * @note      none
 */
static uint8_t a_write_sync(em4095_handle_t *handle)
{
    handle->div_len++;                         /* div length++ */
    if (handle->div_len >= handle->div)        /* wait for div */
    {
        handle->div_len = 0;                   /* init to 0 */

        return 0;                              /* success return 0 */
    }
    else
    {
        return 2;                              /* return error */
    }
}

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
uint8_t em4095_irq_handler(em4095_handle_t *handle)
{
    uint8_t res;
    int64_t diff;
    em4095_time_t t;

    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }

    if (handle->mode == EM4095_MODE_READ)                                      /* read mode */
    {
        uint8_t level;

        if (a_read_sync(handle) != 0)                                          /* wait read sync */
        {
            return 0;                                                          /* success return 0 */
        }
        res = handle->timestamp_read(&t);                                      /* timestamp read */
        if (res != 0)                                                          /* check result */
        {
            handle->debug_print("em4095: timestamp read failed.\n");           /* timestamp read failed */

            return 1;                                                          /* return error */
        }
        res = handle->demod_gpio_read(&level);                                 /* read gpio level */
        if (res != 0)                                                          /* check result */
        {
            handle->debug_print("em4095: demod gpio read failed.\n");          /* demod gpio read failed */

            return 1;                                                          /* return error */
        }
        diff = (int64_t)(t.s - handle->last_time.s) * 1000000 +
               (int64_t)(t.us - handle->last_time.us) ;                        /* now - last time */
        if (diff - (int64_t)200000L >= 0)                                      /* if over 1s, force reset */
        {
            handle->decode_len = 0;                                            /* reset the decode */
        }
        if (handle->decode_len >= (EM4095_MAX_LENGTH - 1))                     /* check the max length */
        {
            handle->decode_len = 0;                                            /* reset the decode */
        }
        handle->decode[handle->decode_len].t.s = t.s;                          /* save s */
        handle->decode[handle->decode_len].t.us = t.us;                        /* save us */
        handle->decode[handle->decode_len].level = level;                      /* save level */
        handle->decode_len++;                                                  /* length++ */
        handle->last_time.s = t.s;                                             /* save last time */
        handle->last_time.us = t.us;                                           /* save last time */
        if (handle->decode_len >= handle->len)                                 /* check length */
        {
            res = handle->shd_gpio_write(1);                                   /* goto sleep mode */
            if (res != 0)                                                      /* check result */
            {
                handle->debug_print("em4095: shd gpio write failed.\n");       /* shd gpio write failed */

                return 1;                                                      /* return error */
            }
            a_em4095_time_diff(handle);                                        /* get time diff */
            handle->receive_callback((em4095_mode_t)handle->mode,
                                     handle->decode,
                                     handle->decode_len);                      /* run the callback */
            handle->mode = EM4095_MODE_IDLE;                                   /* set idle mode */
        }
    }
    else if (handle->mode == EM4095_MODE_WRITE)                                /* write mode */
    {
        uint8_t level;

        if (a_write_sync(handle) != 0)                                         /* wait write sync */
        {
            return 0;                                                          /* success return 0 */
        }
        res = handle->timestamp_read(&t);                                      /* timestamp read */
        if (res != 0)                                                          /* check result */
        {
            handle->debug_print("em4095: timestamp read failed.\n");           /* timestamp read failed */

            return 1;                                                          /* return error */
        }
        handle->decode[handle->len].t.s = t.s;                                 /* save s */
        handle->decode[handle->len].t.us = t.us;                               /* save us */
        level = handle->decode[handle->len].level;                             /* get decode level */
        handle->len++;                                                         /* length++ */
        if (handle->last_bit != level)                                         /* if not the set level */
        {
            res = handle->mod_gpio_write(level);                               /* mod gpio write level */
            if (res != 0)                                                      /* check result */
            {
                handle->debug_print("em4095: mod gpio write failed.\n");       /* mod gpio write */

                return 1;                                                      /* return error */
            }
        }
        handle->last_bit = level;                                              /* save last bit */
        if (handle->len >= handle->decode_len)                                 /* check length */
        {
            res = handle->shd_gpio_write(1);                                   /* goto sleep mode */
            if (res != 0)                                                      /* check result */
            {
                handle->debug_print("em4095: shd gpio write failed.\n");       /* shd gpio write failed */

                return 1;                                                      /* return error */
            }
            a_em4095_time_diff(handle);                                        /* get time diff */
            handle->receive_callback((em4095_mode_t)handle->mode,
                                     handle->decode,
                                     handle->decode_len);                      /* run the callback */
            handle->mode = EM4095_MODE_IDLE;                                   /* set idle mode */
        }
    }
    else
    {
        
    }

    return 0;                                                                  /* success return 0 */
}

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
uint8_t em4095_init(em4095_handle_t *handle)
{
    uint8_t res;
    em4095_time_t t;

    if (handle == NULL)                                                    /* check handle */
    {
        return 2;                                                          /* return error */
    }
    if (handle->debug_print == NULL)                                       /* check debug_print */
    {
        return 3;                                                          /* return error */
    }
    if (handle->demod_gpio_init == NULL)                                   /* check demod_gpio_init */
    {
        handle->debug_print("em4095: demod_gpio_init is null.\n");         /* demod_gpio_init is null */

        return 3;                                                          /* return error */
    }
    if (handle->demod_gpio_deinit == NULL)                                 /* check demod_gpio_deinit */
    {
        handle->debug_print("em4095: demod_gpio_deinit is null.\n");       /* demod_gpio_deinit is null */

        return 3;                                                          /* return error */
    }
    if (handle->demod_gpio_read == NULL)                                   /* check demod_gpio_read */
    {
        handle->debug_print("em4095: demod_gpio_read is null.\n");         /* demod_gpio_read is null */

        return 3;                                                          /* return error */
    }
    if (handle->mod_gpio_init == NULL)                                     /* check mod_gpio_init */
    {
        handle->debug_print("em4095: mod_gpio_init is null.\n");           /* mod_gpio_init is null */

        return 3;                                                          /* return error */
    }
    if (handle->mod_gpio_deinit == NULL)                                   /* check mod_gpio_deinit */
    {
        handle->debug_print("em4095: mod_gpio_deinit is null.\n");         /* mod_gpio_deinit is null */

        return 3;                                                          /* return error */
    }
    if (handle->mod_gpio_write == NULL)                                    /* check mod_gpio_write */
    {
        handle->debug_print("em4095: mod_gpio_write is null.\n");          /* mod_gpio_write is null */

        return 3;                                                          /* return error */
    }
    if (handle->shd_gpio_init == NULL)                                     /* check shd_gpio_init */
    {
        handle->debug_print("em4095: shd_gpio_init is null.\n");           /* shd_gpio_init is null */

        return 3;                                                          /* return error */
    }
    if (handle->shd_gpio_deinit == NULL)                                   /* check shd_gpio_deinit */
    {
        handle->debug_print("em4095: shd_gpio_deinit is null.\n");         /* shd_gpio_deinit is null */

        return 3;                                                          /* return error */
    }
    if (handle->shd_gpio_write == NULL)                                    /* check shd_gpio_write */
    {
        handle->debug_print("em4095: shd_gpio_write is null.\n");          /* shd_gpio_write is null */

        return 3;                                                          /* return error */
    }
    if (handle->timestamp_read == NULL)                                    /* check timestamp_read */
    {
        handle->debug_print("em4095: timestamp_read is null.\n");          /* timestamp_read is null */

        return 3;                                                          /* return error */
    }
    if (handle->delay_ms == NULL)                                          /* check delay_ms */
    {
        handle->debug_print("em4095: delay_ms is null.\n");                /* delay_ms is null */

        return 3;                                                          /* return error */
    }
    if (handle->receive_callback == NULL)                                  /* check receive_callback */
    {
        handle->debug_print("em4095: receive_callback is null.\n");        /* receive_callback is null */

        return 3;                                                          /* return error */
    }

    res = handle->demod_gpio_init();                                       /* demod gpio init */
    if (res != 0)                                                          /* check the result */
    {
        handle->debug_print("em4095: demod gpio init failed.\n");          /* demod gpio init failed */

        return 1;                                                          /* return error */
    }
    res = handle->mod_gpio_init();                                         /* mod gpio init */
    if (res != 0)                                                          /* check the result */
    {
        handle->debug_print("em4095: mod gpio init failed.\n");            /* mod gpio init failed */
        (void)handle->demod_gpio_deinit();                                 /* demod gpio deinit */

        return 1;                                                          /* return error */
    }
    res = handle->shd_gpio_init();                                         /* shd gpio init */
    if (res != 0)                                                          /* check the result */
    {
        handle->debug_print("em4095: shd gpio init failed.\n");            /* shd gpio init failed */
        (void)handle->demod_gpio_deinit();                                 /* demod gpio deinit */
        (void)handle->mod_gpio_deinit();                                   /* mod gpio deinit */

        return 1;                                                          /* return error */
    }
    res = handle->shd_gpio_write(1);                                       /* goto sleep mode */
    if (res != 0)                                                          /* check the result */
    {
        handle->debug_print("em4095: shd gpio write failed.\n");           /* shd gpio write failed */
        (void)handle->demod_gpio_deinit();                                 /* demod gpio deinit */
        (void)handle->mod_gpio_deinit();                                   /* mod gpio deinit */
        (void)handle->shd_gpio_deinit();                                   /* shd gpio deinit */

        return 1;                                                          /* return error */
    }
    res = handle->timestamp_read(&t);                                      /* timestamp read */
    if (res != 0)                                                          /* check result */
    {
        handle->debug_print("em4095: timestamp read failed.\n");           /* timestamp read failed */
        (void)handle->demod_gpio_deinit();                                 /* demod gpio deinit */
        (void)handle->mod_gpio_deinit();                                   /* mod gpio deinit */
        (void)handle->shd_gpio_deinit();                                   /* shd gpio deinit */

        return 1;                                                          /* return error */
    }
    handle->last_time.s = t.s;                                             /* save last time */
    handle->last_time.us = t.us;                                           /* save last time */
    handle->decode_len = 0;                                                /* init 0 */
    handle->mode = 0;                                                      /* set idle mode */
    handle->len = 0;                                                       /* init 0 */
    handle->div = 1;                                                       /* init 1 */
    handle->div_len = 0;                                                   /* init 0 */
    handle->start_flag = 0;                                                /* flag not start */
    handle->last_bit = 0;                                                  /* init 0 */
    handle->inited = 1;                                                    /* flag inited */

    return 0;                                                              /* success return 0 */
}

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
uint8_t em4095_deinit(em4095_handle_t *handle)
{
    uint8_t res;

    if (handle == NULL)                                                  /* check handle */
    {
        return 2;                                                        /* return error */
    }
    if (handle->inited != 1)                                             /* check handle initialization */
    {
        return 3;                                                        /* return error */
    }

    res = handle->shd_gpio_write(1);                                     /* goto sleep mode */
    if (res != 0)                                                        /* check the result */
    {
        handle->debug_print("em4095: shd gpio write failed.\n");         /* shd gpio write failed */

        return 1;                                                        /* return error */
    }
    res = handle->demod_gpio_deinit();                                   /* demod gpio deinit */
    if (res != 0)                                                        /* check the result */
    {
        handle->debug_print("em4095: demod gpio deinit failed.\n");      /* demod gpio deinit failed */

        return 1;                                                        /* return error */
    }
    res = handle->mod_gpio_deinit();                                     /* mod gpio deinit */
    if (res != 0)                                                        /* check the result */
    {
        handle->debug_print("em4095: mod gpio deinit failed.\n");        /* mod gpio deinit failed */

        return 1;                                                        /* return error */
    }
    res = handle->shd_gpio_deinit();                                     /* shd gpio deinit */
    if (res != 0)                                                        /* check the result */
    {
        handle->debug_print("em4095: shd gpio deinit failed.\n");        /* shd gpio deinit failed */

        return 1;                                                        /* return error */
    }
    handle->inited = 0;                                                  /* flag close */

    return 0;                                                            /* success return 0 */
}

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
uint8_t em4095_power_down(em4095_handle_t *handle)
{
    uint8_t res;

    if (handle == NULL)                                                 /* check handle */
    {
        return 2;                                                       /* return error */
    }
    if (handle->inited != 1)                                            /* check handle initialization */
    {
        return 3;                                                       /* return error */
    }

    res = handle->shd_gpio_write(1);                                    /* goto sleep mode */
    if (res != 0)                                                       /* check the result */
    {
        handle->debug_print("em4095: shd gpio write failed.\n");        /* shd gpio write failed */

        return 1;                                                       /* return error */
    }

    return 0;                                                           /* success return 0 */
}

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
uint8_t em4095_power_on(em4095_handle_t *handle)
{
    uint8_t res;

    if (handle == NULL)                                                 /* check handle */
    {
        return 2;                                                       /* return error */
    }
    if (handle->inited != 1)                                            /* check handle initialization */
    {
        return 3;                                                       /* return error */
    }

    res = handle->shd_gpio_write(0);                                    /* exit sleep mode */
    if (res != 0)                                                       /* check the result */
    {
        handle->debug_print("em4095: shd gpio write failed.\n");        /* shd gpio write failed */

        return 1;                                                       /* return error */
    }

    return 0;                                                           /* success return 0 */
}

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
uint8_t em4095_set_div(em4095_handle_t *handle, uint32_t clock_div)
{
    if (handle == NULL)                                          /* check handle */
    {
        return 2;                                                /* return error */
    }
    if (handle->inited != 1)                                     /* check handle initialization */
    {
        return 3;                                                /* return error */
    }
    if (clock_div == 0)                                          /* check the div */
    {
        handle->debug_print("em4095: div can't be 0.\n");        /* div can't be 0 */

        return 4;                                                /* return error */
    }

    handle->div = clock_div;                                     /* set div */

    return 0;                                                    /* success return 0 */
}

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
uint8_t em4095_get_div(em4095_handle_t *handle, uint32_t *clock_div)
{
    if (handle == NULL)             /* check handle */
    {
        return 2;                   /* return error */
    }
    if (handle->inited != 1)        /* check handle initialization */
    {
        return 3;                   /* return error */
    }

    *clock_div = handle->div;       /* set div */

    return 0;                       /* success return 0 */
}

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
uint8_t em4095_read(em4095_handle_t *handle, uint16_t len)
{
    uint8_t res;
    uint8_t level;
    em4095_time_t t;

    if (handle == NULL)                                                 /* check handle */
    {
        return 2;                                                       /* return error */
    }
    if (handle->inited != 1)                                            /* check handle initialization */
    {
        return 3;                                                       /* return error */
    }
    if (len > EM4095_MAX_LENGTH)                                        /* check length */
    {
        handle->debug_print("em4095: len > %d.\n", EM4095_MAX_LENGTH);  /* len is too long */

        return 4;                                                       /* return error */
    }

    res = handle->timestamp_read(&t);                                   /* timestamp read */
    if (res != 0)                                                       /* check result */
    {
        handle->debug_print("em4095: timestamp read failed.\n");        /* timestamp read failed */

        return 1;                                                       /* return error */
    }
    res = handle->mod_gpio_write(0);                                    /* mod gpio write level */
    if (res != 0)                                                       /* check result */
    {
        handle->debug_print("em4095: mod gpio write failed.\n");        /* mod gpio write */

        return 1;                                                       /* return error */
    }
    handle->last_time.s = t.s;                                          /* save last time */
    handle->last_time.us = t.us;                                        /* save last time */
    handle->decode_len = 0;                                             /* init 0 */
    handle->len = len;                                                  /* set read length */
    handle->div_len = 0;                                                /* init 0 */
    handle->mode = EM4095_MODE_READ;                                    /* set read mode */
    handle->start_flag = 0;                                             /* flag not start */
    res = handle->demod_gpio_read(&level);                              /* read gpio level */
    if (res != 0)                                                       /* check result */
    {
        handle->debug_print("em4095: demod gpio read failed.\n");       /* demod gpio read failed */

        return 1;                                                       /* return error */
    }
    handle->last_bit = level;                                           /* init level */

    return 0;                                                           /* success return 0 */
}

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
uint8_t em4095_write(em4095_handle_t *handle, uint8_t *buf, uint16_t len)
{
    uint8_t res;
    uint16_t i;
    em4095_time_t t;

    if (handle == NULL)                                                 /* check handle */
    {
        return 2;                                                       /* return error */
    }
    if (handle->inited != 1)                                            /* check handle initialization */
    {
        return 3;                                                       /* return error */
    }

    if (len > EM4095_MAX_LENGTH)                                        /* check length */
    {
        handle->debug_print("em4095: len > %d.\n", EM4095_MAX_LENGTH);  /* len is too long */

        return 4;                                                       /* return error */
    }

    res = handle->timestamp_read(&t);                                   /* timestamp read */
    if (res != 0)                                                       /* check result */
    {
        handle->debug_print("em4095: timestamp read failed.\n");        /* timestamp read failed */

        return 1;                                                       /* return error */
    }
    res = handle->mod_gpio_write(0);                                    /* mod gpio write level */
    if (res != 0)                                                       /* check result */
    {
        handle->debug_print("em4095: mod gpio write failed.\n");        /* mod gpio write */

        return 1;                                                       /* return error */
    }
    handle->last_time.s = t.s;                                          /* save last time */
    handle->last_time.us = t.us;                                        /* save last time */
    handle->decode_len = 0;                                             /* init 0 */
    for (i = 0; i < len; i++)                                           /* set data */
    {
        handle->decode[handle->decode_len].level = buf[i];              /* save data */
        handle->decode_len++;                                           /* length++ */
    }
    handle->len = 0;                                                    /* set write length */
    handle->div_len = 0;                                                /* init 0 */
    handle->mode = EM4095_MODE_WRITE;                                   /* set write mode */
    handle->start_flag = 0;                                             /* flag not start */
    handle->last_bit = 0;                                               /* init 0 */

    return 0;                                                           /* success return 0 */
}

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
uint8_t em4095_copy_decode_buffer(em4095_handle_t *handle, uint8_t *buf, uint16_t *len)
{
    uint16_t i;

    if (handle == NULL)                             /* check handle */
    {
        return 2;                                   /* return error */
    }
    if (handle->inited != 1)                        /* check handle initialization */
    {
        return 3;                                   /* return error */
    }

    for (i = 0; i < handle->decode_len; i++)        /* copy all */
    {
        buf[i] = handle->decode[i].level;           /* copy to buffer */
    }
    *len = handle->decode_len;                      /* set length */

    return 0;                                       /* success return 0 */
}

/**
 * @brief      get chip's information
 * @param[out] *info points to an em4095 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t em4095_info(em4095_info_t *info)
{
    if (info == NULL)                                               /* check handle */
    {
        return 2;                                                   /* return error */
    }

    memset(info, 0, sizeof(em4095_info_t));                         /* initialize em4095 info structure */
    strncpy(info->chip_name, CHIP_NAME, 32);                        /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);        /* copy manufacturer name */
    strncpy(info->interface, "GPIO", 8);                            /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;                /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;                /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT;                             /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX;                        /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN;                        /* set maximum temperature */
    info->driver_version = DRIVER_VERSION;                          /* set driver version */

    return 0;                                                       /* success return 0 */
}
