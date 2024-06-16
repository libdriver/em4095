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
 * @file      main.c
 * @brief     main source file
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
#include "driver_em4095_read_write_test.h"
#include "shell.h"
#include "clock.h"
#include "delay.h"
#include "gpio.h"
#include "uart.h"
#include "getopt.h"
#include <math.h>
#include <stdlib.h>

/**
 * @brief global var definition
 */
uint8_t g_buf[256];                                      /**< uart buffer */
uint8_t g_rx_buf[256];                                   /**< rx buffer */
volatile uint16_t g_len;                                 /**< uart buffer length */
volatile uint8_t g_flag;                                 /**< interrupt flag */
uint8_t (*g_gpio_irq)(void) = NULL;                      /**< gpio irq */
extern uint8_t em4095_interface_timer_init(void);        /**< timer init function */

/**
 * @brief exti 0 irq
 * @note  none
 */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
 * @brief     gpio exti callback
 * @param[in] pin is the gpio pin
 * @note      none
 */
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin == GPIO_PIN_0)
    {
        /* run the gpio riq */
        if (g_gpio_irq != NULL)
        {
            g_gpio_irq();
        }
    }
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
    switch (mode)
    {
        case EM4095_MODE_READ :
        {
            em4095_interface_debug_print("em4095: irq read done.\n");

            break;
        }
        case EM4095_MODE_WRITE :
        {
            em4095_interface_debug_print("em4095: irq write done.\n");

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
 * @brief     em4095 full function
 * @param[in] argc is arg numbers
 * @param[in] **argv is the arg address
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 *            - 5 param is invalid
 * @note      none
 */
uint8_t em4095(uint8_t argc, char **argv)
{
    int c;
    int longindex = 0;
    char short_options[] = "hipe:t:";
    struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"information", no_argument, NULL, 'i'},
        {"port", no_argument, NULL, 'p'},
        {"example", required_argument, NULL, 'e'},
        {"test", required_argument, NULL, 't'},
        {"data", required_argument, NULL, 1},
        {"length", required_argument, NULL, 2},
        {"times", required_argument, NULL, 3},
        {NULL, 0, NULL, 0},
    };
    char type[33] = "unknown";
    uint32_t times = 3;
    uint32_t length = 256;

    /* if no params */
    if (argc == 1)
    {
        /* goto the help */
        goto help;
    }

    /* init 0 */
    optind = 0;

    /* init 0 */
    memset(g_rx_buf, 0, sizeof(uint8_t) * 256);
    
    /* parse */
    do
    {
        /* parse the args */
        c = getopt_long(argc, argv, short_options, long_options, &longindex);

        /* judge the result */
        switch (c)
        {
            /* help */
            case 'h' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "h");

                break;
            }

            /* information */
            case 'i' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "i");

                break;
            }

            /* port */
            case 'p' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "p");

                break;
            }

            /* example */
            case 'e' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "e_%s", optarg);

                break;
            }

            /* test */
            case 't' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "t_%s", optarg);

                break;
            }

            /* data */
            case 1 :
            {
                char *p;
                uint32_t l;
                uint16_t i;
                uint8_t hex_data;

                /* set the data */
                l = strlen(optarg);

                /* check the header */
                if (l >= 2)
                {
                    if (strncmp(optarg, "0x", 2) == 0)
                    {
                        p = optarg + 2;
                        l -= 2;
                    }
                    else if (strncmp(optarg, "0X", 2) == 0)
                    {
                        p = optarg + 2;
                        l -= 2;
                    }
                    else
                    {
                        p = optarg;
                    }
                }
                else
                {
                    p = optarg;
                }

                /* init 0 */
                hex_data = 0;
                length = 0;

                /* loop */
                for (i = 0; i < l; i++)
                {
                    if ((p[i] <= '9') && (p[i] >= '0'))
                    {
                        hex_data += (p[i] - '0') * (uint32_t)pow(16, (l - i - 1) % 2);
                    }
                    else if ((p[i] <= 'F') && (p[i] >= 'A'))
                    {
                        hex_data += ((p[i] - 'A') + 10) * (uint32_t)pow(16, (l - i - 1) % 2);
                    }
                    else if ((p[i] <= 'f') && (p[i] >= 'a'))
                    {
                        hex_data += ((p[i] - 'a') + 10) * (uint32_t)pow(16, (l - i - 1) % 2);
                    }
                    else
                    {
                        return 5;
                    }
                    if (i % 2 == 1)
                    {
                        g_rx_buf[i / 2] = hex_data & 0xFF;
                        hex_data = 0;
                        length++;
                    }
                }

                break;
            }

            /* data length */
            case 2 :
            {
                /* set the length */
                length = atol(optarg);
                if (length > 256)
                {
                    em4095_interface_debug_print("em4095: length can't be over 256.\n");
                    
                    return 5;
                }

                break;
            }

            /* running times */
            case 3 :
            {
                /* set the times */
                times = atol(optarg);

                break;
            }

            /* the end */
            case -1 :
            {
                break;
            }

            /* others */
            default :
            {
                return 5;
            }
        }
    } while (c != -1);

    /* run the function */
    if (strcmp("t_read", type) == 0)
    {
        uint8_t res;

        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }

        /* set the irq */
        g_gpio_irq = em4095_read_write_test_irq_handler;

        /* run the read write test */
        res = em4095_read_write_test(times);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;

            return 1;
        }

        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;

        return 0;
    }
    else if (strcmp("e_read", type) == 0)
    {
        uint8_t res;
        uint32_t i;

        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }

        /* set the irq */
        g_gpio_irq = em4095_basic_irq_handler;

        /* basic init */
        res = em4095_basic_init(a_receive_callback);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;

            return 1;
        }

        /* read data */
        res = em4095_basic_read(g_rx_buf, length);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            (void)em4095_basic_deinit();
            g_gpio_irq = NULL;

            return 1;
        }

        em4095_interface_debug_print("read data: ");
        for (i = 0; i < length; i++)
        {
            em4095_interface_debug_print("0x%02X ", g_rx_buf[i]);
        }
        em4095_interface_debug_print(".\n");

        /* basic deinit */
        (void)em4095_basic_deinit();

        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;

        return 0;
    }
    else if (strcmp("e_write", type) == 0)
    {
        uint8_t res;
        uint32_t i;

        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }

        /* set the irq */
        g_gpio_irq = em4095_basic_irq_handler;

        /* basic init */
        res = em4095_basic_init(a_receive_callback);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;

            return 1;
        }

        em4095_interface_debug_print("write data: ");
        for (i = 0; i < length; i++)
        {
            em4095_interface_debug_print("0x%02X ", g_rx_buf[i]);
        }
        em4095_interface_debug_print(".\n");

        /* write data */
        res = em4095_basic_write(g_rx_buf, length);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            (void)em4095_basic_deinit();
            g_gpio_irq = NULL;

            return 1;
        }

        /* basic deinit */
        (void)em4095_basic_deinit();

        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;

        return 0;
    }
    else if (strcmp("h", type) == 0)
    {
        help:
        em4095_interface_debug_print("Usage:\n");
        em4095_interface_debug_print("  em4095 (-i | --information)\n");
        em4095_interface_debug_print("  em4095 (-h | --help)\n");
        em4095_interface_debug_print("  em4095 (-p | --port)\n");
        em4095_interface_debug_print("  em4095 (-t readwrite | --test=readwrite) [--times=<num>]\n");
        em4095_interface_debug_print("  em4095 (-e read | --example=read) [--length=<len>]\n");
        em4095_interface_debug_print("  em4095 (-e write | --example=wirte) [--data=<hex>]\n");
        em4095_interface_debug_print("\n");
        em4095_interface_debug_print("Options:\n");
        em4095_interface_debug_print("      --data=<data>              Set the written data.([default: 0])\n");
        em4095_interface_debug_print("  -e <read | write>, --example=<read | write>\n");
        em4095_interface_debug_print("                                 Run the driver example.\n");
        em4095_interface_debug_print("  -h, --help                     Show the help.\n");
        em4095_interface_debug_print("  -i, --information              Show the chip information.\n");
        em4095_interface_debug_print("      --length=<len>             Set the read length.\n");
        em4095_interface_debug_print("  -p, --port                     Display the pin connections of the current board.\n");
        em4095_interface_debug_print("  -t <read>, --test=<read>       Run the driver test.([default: 128])\n");
        em4095_interface_debug_print("      --times=<num>              Set the running times.([default: 3])\n");

        return 0;
    }
    else if (strcmp("i", type) == 0)
    {
        em4095_info_t info;

        /* print em4095 info */
        em4095_info(&info);
        em4095_interface_debug_print("em4095: chip is %s.\n", info.chip_name);
        em4095_interface_debug_print("em4095: manufacturer is %s.\n", info.manufacturer_name);
        em4095_interface_debug_print("em4095: interface is %s.\n", info.interface);
        em4095_interface_debug_print("em4095: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        em4095_interface_debug_print("em4095: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        em4095_interface_debug_print("em4095: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        em4095_interface_debug_print("em4095: max current is %0.2fmA.\n", info.max_current_ma);
        em4095_interface_debug_print("em4095: max temperature is %0.1fC.\n", info.temperature_max);
        em4095_interface_debug_print("em4095: min temperature is %0.1fC.\n", info.temperature_min);

        return 0;
    }
    else if (strcmp("p", type) == 0)
    {
        /* print pin connection */
        em4095_interface_debug_print("em4095: CLK connected to GPIOB PIN0.\n");
        em4095_interface_debug_print("em4095: SHD connected to GPIOB PIN2.\n");
        em4095_interface_debug_print("em4095: DEMOD_OUT connected to GPIOA PIN8.\n");
        em4095_interface_debug_print("em4095: MOD connected to GPIOB PIN1.\n");

        return 0;
    }
    else
    {
        return 5;
    }
}

/**
 * @brief main function
 * @note  none
 */
int main(void)
{
    uint8_t res;

    /* stm32f407 clock init and hal init */
    clock_init();

    /* delay init */
    delay_init();

    /* uart init */
    uart_init(115200);

    /* timer init */
    em4095_interface_timer_init();

    /* shell init && register em4095 function */
    shell_init();
    shell_register("em4095", em4095);
    uart_print("em4095: welcome to libdriver em4095.\n");

    while (1)
    {
        /* read uart */
        g_len = uart_read(g_buf, 256);
        if (g_len != 0)
        {
            /* run shell */
            res = shell_parse((char *)g_buf, g_len);
            if (res == 0)
            {
                /* run success */
            }
            else if (res == 1)
            {
                uart_print("em4095: run failed.\n");
            }
            else if (res == 2)
            {
                uart_print("em4095: unknown command.\n");
            }
            else if (res == 3)
            {
                uart_print("em4095: length is too long.\n");
            }
            else if (res == 4)
            {
                uart_print("em4095: pretreat failed.\n");
            }
            else if (res == 5)
            {
                uart_print("em4095: param is invalid.\n");
            }
            else
            {
                uart_print("em4095: unknown status code.\n");
            }
            uart_flush();
        }
        delay_ms(100);
    }
}
