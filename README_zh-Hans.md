[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver EM4095

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/em4095/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

EM4095（以前称为P4095）芯片是一种CMOS集成收发器电路，用于RFID基站，以执行以下功能：

- 用载波频率驱动天线。
- 可写转发器的AM调制字段。
- 发射机应答器引起的天线信号调制的AM解调通过简单的接口与微处理器通信。

LibDriver EM4095是LibDriver推出的EM4095全功能驱动，该驱动提供RF读取，RF写入等功能并且它符合MISRA标准。

### 目录

  - [说明](#说明)
  - [安装](#安装)
  - [使用](#使用)
    - [example basic](#example-basic)
  - [文档](#文档)
  - [贡献](#贡献)
  - [版权](#版权)
  - [联系我们](#联系我们)

### 说明

/src目录包含了LibDriver EM4095的源文件。

/interface目录包含了LibDriver EM4095与平台无关的GPIO总线模板。

/test目录包含了LibDriver EM4095驱动测试程序，该程序可以简单的测试芯片必要功能。

/example目录包含了LibDriver EM4095编程范例。

/doc目录包含了LibDriver EM4095离线文档。

/datasheet目录包含了EM4095数据手册。

/project目录包含了常用Linux与单片机开发板的工程样例。所有工程均采用shell脚本作为调试方法，详细内容可参考每个工程里面的README.md。

/misra目录包含了LibDriver MISRA代码扫描结果。

### 安装

参考/interface目录下与平台无关的GPIO总线模板，完成指定平台的GPIO总线驱动。

将/src目录，您使用平台的接口驱动和您开发的驱动加入工程，如果您想要使用默认的范例驱动，可以将/example目录加入您的工程。

### 使用

您可以参考/example目录下的编程范例完成适合您的驱动，如果您想要使用默认的编程范例，以下是它们的使用方法。

#### example basic

```C
#include "driver_em4095_basic.h"

uint8_t res;
uint32_t i;
uint8_t g_rx_buf[256];
uint32_t length = 256;

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

...
    
/* gpio init */
res = gpio_interrupt_init();
if (res != 0)
{
    return 1;
}

/* set the irq */
g_gpio_irq = em4095_basic_irq_handler;

...
    
/* basic init */
res = em4095_basic_init(a_receive_callback);
if (res != 0)
{
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;
}

...
    
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

...
    
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

...
    
/* basic deinit */
(void)em4095_basic_deinit();

/* gpio deinit */
(void)gpio_interrupt_deinit();
g_gpio_irq = NULL;

return 0;
```

### 文档

在线文档: [https://www.libdriver.com/docs/em4095/index.html](https://www.libdriver.com/docs/em4095/index.html)。

离线文档: /doc/html/index.html。

### 贡献

请参考CONTRIBUTING.md。

### 版权

版权 (c) 2015 - 现在 LibDriver 版权所有

MIT 许可证（MIT）

特此免费授予任何获得本软件副本和相关文档文件（下称“软件”）的人不受限制地处置该软件的权利，包括不受限制地使用、复制、修改、合并、发布、分发、转授许可和/或出售该软件副本，以及再授权被配发了本软件的人如上的权利，须在下列条件下：

上述版权声明和本许可声明应包含在该软件的所有副本或实质成分中。

本软件是“如此”提供的，没有任何形式的明示或暗示的保证，包括但不限于对适销性、特定用途的适用性和不侵权的保证。在任何情况下，作者或版权持有人都不对任何索赔、损害或其他责任负责，无论这些追责来自合同、侵权或其它行为中，还是产生于、源于或有关于本软件以及本软件的使用或其它处置。

### 联系我们

请联系lishifenging@outlook.com。