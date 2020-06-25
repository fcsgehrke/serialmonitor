#ifndef APP_H_INCLUDED
#define APP_H_INCLUDED

#include <string.h>

#include "ui.h"
#include "lib/rs232.h"
#include "lib/ringbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*OnRxData)(char *data, uint8_t sz);
typedef void (*OnTxData)(char *data, uint8_t sz);
typedef void (*OnNewData)(void);

typedef struct
{
    uint8_t Connected;
    char PortName[5];
    uint8_t PortIndex;
    uint8_t Outputs[4];
    uint8_t Inputs[4];
    OnRxData RxEvent;
    OnTxData TxEvent;
    OnNewData NewDataEvent;
} AppData;

extern AppData appData;

void app_init(void);
void app_set_on_rx_data(OnRxData rx);
void app_set_on_tx_data(OnRxData tx);
void app_set_on_new_data(OnNewData nd);
uint8_t app_connect(uint8_t port);
uint8_t app_get_ports(char ports[][5]);
void app_disconnect(void);
void app_toggle_output(uint8_t output);
void app_auto_read_io(uint8_t enable);
void app_write_lcd(char *data, uint8_t line);
void app_clear_lcd(void);

#define START_BYTE          0x02
#define END_BYTE            0x03
#define IO_WRITE_BYTE       0x01
#define IO_READ_BYTE        0x02

#ifdef __cplusplus
}
#endif

#endif // APP_H_INCLUDED
