#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdint.h>
#include "resource.h"

#include "app.h"

#define IDT_TIMER_LEDS  60000
#define IDT_TIMER_IO_UPDATE  60001

#ifdef __cplusplus
extern "C" {
#endif

#define WM_NEW_DATA (WM_USER + 0x0001)

// App Instance
extern HINSTANCE hInst;

// Icon Handles
extern HICON hIconOff;
extern HICON hIconOn;

// Control Handles
extern HWND hLog;
extern HWND hCombo;

// LED Blink Control
extern uint8_t led_tx;
extern uint8_t led_rx;

void loadIcons(HWND hwndDlg);
void loadControls(HWND hwndDlg);
void setOutputData(HWND hwndDlg, uint8_t output, uint8_t value);
void setInputData(HWND hwndDlg, uint8_t input, uint8_t value);
void addLogLine(char *line);
void log(const char *txt, uint8_t serial, uint8_t tx);
void clearLog(void);
void addComboData(const char *data);
void clearComboData(void);
void setTxState(HWND hwndDlg, uint8_t state);
void setRxState(HWND hwndDlg, uint8_t state);
void blinkTx(HWND hwndDlg);
void blinkRx(HWND hwndDlg);
void initTimers(HWND hwndDlg);
void changeUiState(HWND hwndDlg, uint8_t enabled);

#define LOG(txt) \
    log(txt, 0, 0);

#define LOG_TX(txt) \
    log(txt, 1, 1);

#define LOG_RX(txt) \
    log(txt, 1, 0);

BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#ifdef __cplusplus
}
#endif

#endif // UI_H_INCLUDED
