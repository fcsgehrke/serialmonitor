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
        break;
    case 1:
        SendDlgItemMessage(hwndDlg, IDD_OUTPUT_2_LED, STM_SETIMAGE, IMAGE_ICON, value == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
        break;
    case 2:
        SendDlgItemMessage(hwndDlg, IDD_OUTPUT_3_LED, STM_SETIMAGE, IMAGE_ICON, value == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
        break;
    case 3:
        SendDlgItemMessage(hwndDlg, IDD_OUTPUT_4_LED, STM_SETIMAGE, IMAGE_ICON, value == 1 ? (LPARAM)hIconOn : (LPARAM)hIconOff);
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

void initTimer(HWND hwndDlg)
{
    SetTimer(hwndDlg, IDT_TIMER_LEDS, 150, (TIMERPROC) NULL);
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

void updateUi(HWND hwndDlg)
{
    for (int i = 0; i < 4; i++)
    {
        setInputData(hwndDlg, i, appData.Inputs[i]);
        setOutputData(hwndDlg, i, appData.Outputs[i]);
    }
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
                int dwPos = SendDlgItemMessage(hwndDlg, IDD_PWM_SLIDER, TBM_GETPOS, 0, 0);
                printf("Position: %d\n", dwPos);

            }
            break;

        case WM_INITDIALOG:
        {
            loadIcons(hwndDlg);
            loadControls(hwndDlg);
            initTimer(hwndDlg);

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
            return TRUE;

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDD_OUTPUT_1_BTN:
                LOG("Clicked !");
                setOutputData(hwndDlg, 0, 1);
                addComboData("COM1");
                blinkRx(hwndDlg);
                break;

            case IDD_OUTPUT_2_BTN:
                LOG_RX("Clicked !");
                setOutputData(hwndDlg, 1, 1);
                 addComboData("COM2");
                 blinkTx(hwndDlg);
                break;

            case IDD_OUTPUT_3_BTN:
                LOG_TX("Clicked !");
                setOutputData(hwndDlg, 2, 1);
                clearComboData();
                break;

            case IDD_OUTPUT_4_BTN:
                setOutputData(hwndDlg, 3, 1);
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
                    int ret = DialogBox(hInst, MAKEINTRESOURCE(DLG_LCD), hwndDlg, (DLGPROC)DlgMain);
                }
                break;

            case IDD_PWM_SLIDER:
                printf("pwm...\n");
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
                        break;
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

