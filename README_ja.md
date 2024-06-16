[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver EM4095

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/em4095/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

EM4095 (旧称 P4095) チップは、RFID 基地局での使用を目的とした CMOS 統合トランシーバー回路で、次の機能を実行します:

- キャリア周波数によるアンテナ駆動。
- 書き込み可能なトランスポンダーのフィールドの AM 変調。
- トランスポンダーによって誘導されるアンテナ信号変調の AM 復調は、シンプルなインターフェイスを介してマイクロプロセッサーと通信します。

LibDriver EM4095 は、LibDriver がリリースした EM4095 のフル機能ドライバーです。RF 読み取り、RF 書き込みなどの機能を提供します。LibDriver は MISRA に準拠しています。

### 目次

  - [説明](#説明)
  - [インストール](#インストール)
  - [使用](#使用)
    - [example basic](#example-basic)
  - [ドキュメント](#ドキュメント)
  - [貢献](#貢献)
  - [著作権](#著作権)
  - [連絡して](#連絡して)

### 説明

/ srcディレクトリには、LibDriver EM4095のソースファイルが含まれています。

/ interfaceディレクトリには、LibDriver EM4095用のプラットフォームに依存しないGPIOバステンプレートが含まれています。

/ testディレクトリには、チップの必要な機能を簡単にテストできるLibDriver EM4095ドライバーテストプログラムが含まれています。

/ exampleディレクトリには、LibDriver EM4095プログラミング例が含まれています。

/ docディレクトリには、LibDriver EM4095オフラインドキュメントが含まれています。

/ datasheetディレクトリには、EM4095データシートが含まれています。

/ projectディレクトリには、一般的に使用されるLinuxおよびマイクロコントローラー開発ボードのプロジェクトサンプルが含まれています。 すべてのプロジェクトは、デバッグ方法としてシェルスクリプトを使用しています。詳細については、各プロジェクトのREADME.mdを参照してください。

/ misraはLibDriver misraコードスキャン結果を含む。

### インストール

/ interfaceディレクトリにあるプラットフォームに依存しないGPIOバステンプレートを参照して、指定したプラットフォームのGPIOバスドライバを完成させます。

/src ディレクトリ、プラットフォームのインターフェイス ドライバー、および独自のドライバーをプロジェクトに追加します。デフォルトのサンプル ドライバーを使用する場合は、/example ディレクトリをプロジェクトに追加します。

### 使用

/example ディレクトリ内のサンプルを参照して、独自のドライバーを完成させることができます。 デフォルトのプログラミング例を使用したい場合の使用方法は次のとおりです。

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

### ドキュメント

オンラインドキュメント: [https://www.libdriver.com/docs/em4095/index.html](https://www.libdriver.com/docs/em4095/index.html)。

オフラインドキュメント: /doc/html/index.html。

### 貢献

CONTRIBUTING.mdを参照してください。

### 著作権

著作権（c）2015-今 LibDriver 全著作権所有

MITライセンス（MIT）

このソフトウェアおよび関連するドキュメントファイル（「ソフトウェア」）のコピーを取得した人は、無制限の使用、複製、変更、組み込み、公開、配布、サブライセンスを含む、ソフトウェアを処分する権利を制限なく付与されます。ソフトウェアのライセンスおよび/またはコピーの販売、および上記のようにソフトウェアが配布された人の権利のサブライセンスは、次の条件に従うものとします。

上記の著作権表示およびこの許可通知は、このソフトウェアのすべてのコピーまたは実体に含まれるものとします。

このソフトウェアは「現状有姿」で提供され、商品性、特定目的への適合性、および非侵害の保証を含むがこれらに限定されない、明示または黙示を問わず、いかなる種類の保証もありません。 いかなる場合も、作者または著作権所有者は、契約、不法行為、またはその他の方法で、本ソフトウェアおよび本ソフトウェアの使用またはその他の廃棄に起因または関連して、請求、損害、またはその他の責任を負わないものとします。

### 連絡して

お問い合わせくださいlishifenging@outlook.com。