#include "ui.h"

// App Instance
HINSTANCE hInst;

// Window Instance
HWND hMainDlg = 0;

// Icon Handles
HICON hIconOff;
HICON hIconOn;

// Control Handles
HWND hLog;
HWND hCombo;
HWND hBtnConnect;

// LED Blink Control
uint8_t led_tx;
uint8_t led_rx;

uint8_t port_count = 0;
char port_names[10][5];
char buf_conv[4096];
uint8_t auto_io = 0;

// LCD Dialog
char lcd_text[16];
uint8_t lcd_col, lcd_row;

void loadIcons(HWND hwndDlg)
{
    HICON hIcon;
    hIcon = (HICON)LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDI_MAIN_ICON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
    if (hIcon)
        SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    hIconOff = (HICON)LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDI_IO_OFF), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
    hIconOn = (HICON)LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDI_IO_ON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
}

void loadControls(HWND hwndDlg)
{
   hLog = GetDlgItem (hwndDlg, IDD_LOG);
   hCombo = GetDlgItem(hwndDlg, IDD_PORT_COMBO);
   hBtnConnect = GetDlgItem(hwndDlg, IDD_CONNECT_BTN);

   SendDlgItemMessage(hwndDlg, IDD_PWM_SLIDER, TBM_SETRANGE, (WPARAM)1, MAKELONG(0, 100));
}

void setOutputData(HWND hwndDlg, uint8_t output, uint8_t value)
{
    switch(output)
    {
    case 0:
        SendDlgItemMessage(hwndDlg, IDD_OUTPUT_1_LED, STM_SETIMAGE, IMAGE_ICON, value == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
        SetDlgItemText(hwndDlg, IDD_OUTPUT_1_BTN, value == 1 ? "Desliga Saida 1" : "Liga Saida 1");
        break;
    case 1:
        SendDlgItemMessage(hwndDlg, IDD_OUTPUT_2_LED, STM_SETIMAGE, IMAGE_ICON, value == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
        SetDlgItemText(hwndDlg, IDD_OUTPUT_2_BTN, value == 1 ? "Desliga Saida 2" : "Liga Saida 2");
        break;
    case 2:
        SendDlgItemMessage(hwndDlg, IDD_OUTPUT_3_LED, STM_SETIMAGE, IMAGE_ICON, value == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
        SetDlgItemText(hwndDlg, IDD_OUTPUT_3_BTN, value == 1 ? "Desliga Saida 3" : "Liga Saida 3");
        break;
    case 3:
        SendDlgItemMessage(hwndDlg, IDD_OUTPUT_4_LED, STM_SETIMAGE, IMAGE_ICON, value == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
        SetDlgItemText(hwndDlg, IDD_OUTPUT_4_BTN, value == 1 ? "Desliga Saida 4" : "Liga Saida 4");
        break;
    }
}

void setInputData(HWND hwndDlg, uint8_t input, uint8_t value)
{
    switch(input)
    {
    case 0:
        SendDlgItemMessage(hwndDlg, IDD_INPUT_1_LED, STM_SETIMAGE, IMAGE_ICON, value == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
        break;
    case 1:
        SendDlgItemMessage(hwndDlg, IDD_INPUT_2_LED, STM_SETIMAGE, IMAGE_ICON, value == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
        break;
    case 2:
        SendDlgItemMessage(hwndDlg, IDD_INPUT_3_LED, STM_SETIMAGE, IMAGE_ICON, value == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
        break;
    case 3:
        SendDlgItemMessage(hwndDlg, IDD_INPUT_4_LED, STM_SETIMAGE, IMAGE_ICON, value == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
        break;
    }
}

void addLogLine(char *line)
{
    int index = GetWindowTextLength (hLog);
    SetFocus (hLog); // set focus
    SendMessageA(hLog, EM_SETSEL, (WPARAM)index, (LPARAM)index); // set selection - end of text
    SendMessageA(hLog, EM_REPLACESEL, 0, (LPARAM)line); // append!
}

void log(const char *txt, uint8_t serial, uint8_t tx)
{
    char line[1024];
    SYSTEMTIME SystemTime;

    GetSystemTime(&SystemTime);
    if (!serial)
        sprintf(line, "%02d:%02d:%02d.%04d - %s\r\n", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds, txt);
    else
    {
        if (tx)
            sprintf(line, "%02d:%02d:%02d.%04d - TX - %s\r\n", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds, txt);
        else
            sprintf(line, "%02d:%02d:%02d.%04d - RX - %s\r\n", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds, txt);
    }

    addLogLine(line);
}

void clearLog(void)
{
     SendMessageA(hLog, WM_SETTEXT, 0, (LPARAM)"");
}

void addComboData(const char *data)
{
    SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM)data);
}

void clearComboData(void)
{
    SendMessageA(hCombo, CB_RESETCONTENT, 0, (LPARAM)"");
}

void setTxState(HWND hwndDlg, uint8_t state)
{
    SendDlgItemMessage(hwndDlg, IDD_TX_LED, STM_SETIMAGE, IMAGE_ICON, state == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
}

void setRxState(HWND hwndDlg, uint8_t state)
{
    SendDlgItemMessage(hwndDlg, IDD_RX_LED, STM_SETIMAGE, IMAGE_ICON, state == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
}

void blinkTx(HWND hwndDlg)
{
    setTxState(hwndDlg, 1);
    led_tx = 1;
}

void blinkRx(HWND hwndDlg)
{
    setRxState(hwndDlg, 1);
    led_rx = 1;
}

void setPorts(int count)
{
    for (int i = 0; i < count; i++)
    {
        addComboData(&port_names[i][0]);
    }
}

void initTimers(HWND hwndDlg)
{
    SetTimer(hwndDlg, IDT_TIMER_LEDS, 150, (TIMERPROC) NULL);
    SetTimer(hwndDlg, IDT_TIMER_IO_UPDATE, 100, (TIMERPROC) NULL);
}

void conv_byte_to_hexasc(char data, char *dest)
{
    if ((data & 0x0F) >= 0x0A)
        dest[1] = (data & 0x0F) + 0x41;
    else
        dest[1] = (data & 0x0F) + 0x30;

    data >>= 4;
    if ((data & 0x0F) >= 0x0A)
        dest[0] = data + 0x41;
    else
        dest[0] = data + 0x30;
}

void uiOnRxData(char *data, uint8_t sz)
{
    int idx = 0;
    char asc_byte[2];

    for (int i = 0; i < sz; i++)
    {
        conv_byte_to_hexasc(*data, asc_byte);
        buf_conv[idx] = asc_byte[0];
        buf_conv[idx+1] = asc_byte[1];
        buf_conv[idx+2] = ' ';
        idx += 3;
        data++;
    }
    buf_conv[idx] = 0;
    LOG_RX(buf_conv);

    blinkRx(hMainDlg);
}

void uiOnTxData(char *data, uint8_t sz)
{
    int idx = 0;
    char asc_byte[2];

    for (int i = 0; i < sz; i++)
    {
        conv_byte_to_hexasc(*data, asc_byte);
        buf_conv[idx] = asc_byte[0];
        buf_conv[idx+1] = asc_byte[1];
        buf_conv[idx+2] = ' ';
        idx += 3;
        data++;
    }
    buf_conv[idx] = 0;
    LOG_TX(buf_conv);

    blinkTx(hMainDlg);
}

void uiOnNewData(void)
{
    SendMessage(hMainDlg, WM_NEW_DATA, 0, (LPARAM)"");
}

void setPwmData(HWND hwndDlg, uint8_t pwm)
{
    char str[20];
    sprintf(str, "Valor: %3d", pwm);
    SetDlgItemText(hwndDlg, IDD_PWM_VALUE, str);
}

void updateUi(HWND hwndDlg)
{
    for (int i = 0; i < 4; i++)
    {
        setInputData(hwndDlg, i, appData.Inputs[i]);
        setOutputData(hwndDlg, i, appData.Outputs[i]);
    }
    setPwmData(hwndDlg, appData.Pwm);
}

void clearIO(HWND hwndDlg)
{
    for (int i = 0; i < 4; i++)
    {
        setInputData(hwndDlg, i, 0);
        setOutputData(hwndDlg, i, 0);
    }
}

void changeUiState(HWND hwndDlg, uint8_t enabled)
{
    EnableWindow(GetDlgItem(hwndDlg, IDD_AUTO_READ_IO_BTN), enabled);
    EnableWindow(GetDlgItem(hwndDlg, IDD_CLEAR_LCD_BTN), enabled);
    EnableWindow(GetDlgItem(hwndDlg, IDD_OUTPUT_1_BTN), enabled);
    EnableWindow(GetDlgItem(hwndDlg, IDD_OUTPUT_2_BTN), enabled);
    EnableWindow(GetDlgItem(hwndDlg, IDD_OUTPUT_3_BTN), enabled);
    EnableWindow(GetDlgItem(hwndDlg, IDD_OUTPUT_4_BTN), enabled);
    EnableWindow(GetDlgItem(hwndDlg, IDD_READ_IO_BTN), enabled);
    EnableWindow(GetDlgItem(hwndDlg, IDD_WRITE_LCD_BTN), enabled);
    EnableWindow(GetDlgItem(hwndDlg, IDD_PWM_SLIDER), enabled);

    EnableWindow(GetDlgItem(hwndDlg, IDD_PORT_COMBO), !enabled);
}

BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    uint8_t pwm = 0, sz;
    uint8_t outputs_aux[4];
    char aux[2];

    if (hMainDlg == 0 || hwndDlg == hMainDlg)
    {
        switch(uMsg)
        {
        case WM_NEW_DATA:
            updateUi(hwndDlg);
        return TRUE;

        case WM_HSCROLL:
            if (LOWORD(wParam) == TB_THUMBTRACK)
            {
                pwm = SendDlgItemMessage(hwndDlg, IDD_PWM_SLIDER, TBM_GETPOS, 0, 0);
                if (pwm % 5 == 0)
                    app_set_pwm(pwm);
            }
            break;

        case WM_INITDIALOG:
        {
            loadIcons(hwndDlg);
            loadControls(hwndDlg);
            initTimers(hwndDlg);

            app_init();

            app_set_on_rx_data(&uiOnRxData);
            app_set_on_tx_data(&uiOnTxData);
            app_set_on_new_data(&uiOnNewData);

            LOG("Sistema Inicializado.");

            hMainDlg = hwndDlg;
        }
        return TRUE;

        case WM_CLOSE:
        {
            app_disconnect();
            EndDialog(hwndDlg, 0);
        }
        return TRUE;

        case WM_TIMER:
            if (LOWORD(wParam) == IDT_TIMER_LEDS)
            {
                if (led_tx)
                {
                    led_tx = 0;
                    setTxState(hwndDlg, 0);
                }
                if (led_rx)
                {
                    led_rx = 0;
                    setRxState(hwndDlg, 0);
                }
            }
            else if (LOWORD(wParam) == IDT_TIMER_IO_UPDATE)
            {
                if (auto_io)
                    app_read_io();
            }
            return TRUE;

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDD_OUTPUT_1_BTN:
                memcpy(outputs_aux, appData.Outputs, 4);
                outputs_aux[0] = !outputs_aux[0];
                app_write_outputs(outputs_aux);
                break;

            case IDD_OUTPUT_2_BTN:
                memcpy(outputs_aux, appData.Outputs, 4);
                outputs_aux[1] = !outputs_aux[1];
                app_write_outputs(outputs_aux);
                break;

            case IDD_OUTPUT_3_BTN:
                memcpy(outputs_aux, appData.Outputs, 4);
                outputs_aux[2] = !outputs_aux[2];
                app_write_outputs(outputs_aux);
                break;

            case IDD_OUTPUT_4_BTN:
                memcpy(outputs_aux, appData.Outputs, 4);
                outputs_aux[3] = !outputs_aux[3];
                app_write_outputs(outputs_aux);
                break;

            case IDD_AUTO_READ_IO_BTN:
                if (appData.Connected)
                {
                    if (auto_io)
                    {
                        auto_io = 0;
                        SetDlgItemText(hwndDlg, IDD_AUTO_READ_IO_BTN, "Ler I/O Automático");
                    }
                    else
                    {
                        auto_io = 1;
                        SetDlgItemText(hwndDlg, IDD_AUTO_READ_IO_BTN, "Desligar I/O Automático");
                    }
                }
                break;

            case IDD_CLEAR_LCD_BTN:
                app_clear_lcd();
                break;

            case IDD_LOG_CLEAR_BTN:
                clearLog();
                break;

            case IDD_PORT_COMBO:
                if (HIWORD(wParam) == CBN_DROPDOWN)
                {
                    clearComboData();

                    port_count = app_get_ports(port_names);
                    if (port_count > 0)
                        setPorts(port_count);
                }
                break;

            case IDD_CONNECT_BTN:
                if (appData.Connected)
                {
                    clearComboData();
                    SetWindowText(hBtnConnect, "Conectar");
                    app_disconnect();
                    LOG("Porta desconectada.");

                    clearIO(hwndDlg);
                    changeUiState(hwndDlg, 0);

                    auto_io = 0;
                    SetDlgItemText(hwndDlg, IDD_AUTO_READ_IO_BTN, "Ler I/O Automático");
                }
                else
                {
                    int idx = SendMessageA(hCombo, CB_GETCURSEL, 0, (LPARAM)"");

                    if (idx != CB_ERR)
                    {
                        if (app_connect(idx))
                        {
                            SetWindowText(hBtnConnect, "Desconectar");
                            LOG("Porta conectada.");

                            blinkRx(hwndDlg);
                            blinkTx(hwndDlg);
                            changeUiState(hwndDlg, 1);
                        }
                        else
                            LOG("ERRO: Não foi possivel conectar a porta selecionada.");
                    }
                    else
                        LOG("ERRO: Você deve selecionar uma porta antes de conectar !");

                }
                break;

            case IDD_WRITE_LCD_BTN:
                if (HIWORD(wParam) == BN_CLICKED)
                {
                    DialogBox(hInst, MAKEINTRESOURCE(DLG_LCD), hwndDlg, (DLGPROC)DlgMain);
                }
                break;
            }
        }
        return TRUE;
        }
    }
    else
    {
        switch(uMsg)
        {
            case WM_INITDIALOG:
            {

            }
            return TRUE;

            case WM_COMMAND:
            {
                switch(LOWORD(wParam))
                {
                    case IDD_LCD_CANCEL_BTN:
                        EndDialog(hwndDlg, 0);
                        break;

                    case IDD_LCD_SEND_BTN:
                        sz = GetDlgItemText(hwndDlg, IDD_LCD_DATA, lcd_text, 16);
                        GetDlgItemText(hwndDlg, IDD_LCD_ROW, aux, 2);
                        lcd_row = atoi(aux);
                        if (lcd_row > 1)
                            lcd_row = 1;
                        GetDlgItemText(hwndDlg, IDD_LCD_COL, aux, 2);
                        lcd_col = atoi(aux);
                        app_write_lcd(lcd_text, sz, lcd_col, lcd_row);
                        EndDialog(hwndDlg, 0);
                        break;
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

