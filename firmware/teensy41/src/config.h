#ifndef CONFIG_H
#define CONFIG_H

#define SD_CS_PIN      BUILTIN_SDCARD
#define TFT_CS         10
#define TFT_DC         9
#define TFT_RST        8
#define TFT_LED        5
#define ONBOARD_LED    13
#define EXTERNAL_LED   12

#define MAX_LINES      15
#define LINE_HEIGHT    16
#define MAX_CMD        128

#define USB_HOST_BAUD  115200
#define UART_BAUD      921600
#define UART_RX_PIN    0
#define UART_TX_PIN    1

#endif
