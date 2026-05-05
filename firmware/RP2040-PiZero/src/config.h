#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions (safe names to avoid conflicts)
#define DOOMSDAY_SD_CS_PIN      BUILTIN_SDCARD
#define DOOMSDAY_TFT_CS         10
#define DOOMSDAY_TFT_DC         9
#define DOOMSDAY_TFT_RST        8
#define DOOMSDAY_TFT_LED        5
#define DOOMSDAY_ONBOARD_LED    13
#define DOOMSDAY_EXTERNAL_LED   12

// Terminal settings
#define DOOMSDAY_MAX_LINES      15
#define DOOMSDAY_LINE_HEIGHT    16
#define DOOMSDAY_MAX_CMD        128

// Baud rates
#define DOOMSDAY_USB_HOST_BAUD  115200
#define DOOMSDAY_UART_BAUD      921600

// UART pins (if using HardwareSerial)
#define DOOMSDAY_UART_RX_PIN    0
#define DOOMSDAY_UART_TX_PIN    1

// Doomsday Message Bus (UART between boards)
#define DMB_SERIAL          Serial1
#define DMB_BAUD_RATE       DOOMSDAY_UART_BAUD


#endif