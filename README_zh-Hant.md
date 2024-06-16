[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver EM4095

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/em4095/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

EM4095（以前稱為P4095）晶片是一種CMOS集成收發器電路，用於RFID基站，以執行以下功能：

- 用載波頻率驅動天線。
- 可寫轉發器的AM調製欄位。
- 發射機應答器引起的天線訊號調製的AM解調通過簡單的介面與微處理器通信。

LibDriver EM4095是LibDriver推出的EM4095全功能驅動，該驅動提供RF讀取，RF寫入等功能並且它符合MISRA標準。

### 目錄

  - [說明](#說明)
  - [安裝](#安裝)
  - [使用](#使用)
    - [example basic](#example-basic)
  - [文檔](#文檔)
  - [貢獻](#貢獻)
  - [版權](#版權)
  - [聯繫我們](#聯繫我們)

### 說明

/src目錄包含了LibDriver EM4095的源文件。

/interface目錄包含了LibDriver EM4095與平台無關的GPIO總線模板。

/test目錄包含了LibDriver EM4095驅動測試程序，該程序可以簡單的測試芯片必要功能。

/example目錄包含了LibDriver EM4095編程範例。

/doc目錄包含了LibDriver EM4095離線文檔。

/datasheet目錄包含了EM4095數據手冊。

/project目錄包含了常用Linux與單片機開發板的工程樣例。所有工程均採用shell腳本作為調試方法，詳細內容可參考每個工程裡面的README.md。

/misra目錄包含了LibDriver MISRA程式碼掃描結果。

### 安裝

參考/interface目錄下與平台無關的GPIO總線模板，完成指定平台的GPIO總線驅動。

將/src目錄，您使用平臺的介面驅動和您開發的驅動加入工程，如果您想要使用默認的範例驅動，可以將/example目錄加入您的工程。

### 使用

您可以參考/example目錄下的程式設計範例完成適合您的驅動，如果您想要使用默認的程式設計範例，以下是它們的使用方法。

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

### 文檔

在線文檔: [https://www.libdriver.com/docs/em4095/index.html](https://www.libdriver.com/docs/em4095/index.html)。

離線文檔: /doc/html/index.html。

### 貢獻

請參攷CONTRIBUTING.md。

### 版權

版權 (c) 2015 - 現在 LibDriver 版權所有

MIT 許可證（MIT）

特此免費授予任何獲得本軟件副本和相關文檔文件（下稱“軟件”）的人不受限制地處置該軟件的權利，包括不受限制地使用、複製、修改、合併、發布、分發、轉授許可和/或出售該軟件副本，以及再授權被配發了本軟件的人如上的權利，須在下列條件下：

上述版權聲明和本許可聲明應包含在該軟件的所有副本或實質成分中。

本軟件是“如此”提供的，沒有任何形式的明示或暗示的保證，包括但不限於對適銷性、特定用途的適用性和不侵權的保證。在任何情況下，作者或版權持有人都不對任何索賠、損害或其他責任負責，無論這些追責來自合同、侵權或其它行為中，還是產生於、源於或有關於本軟件以及本軟件的使用或其它處置。

### 聯繫我們

請聯繫lishifenging@outlook.com。
