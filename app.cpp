#include "app.h"



// Read Thread
DWORD WINAPI ReadThreadFunc(LPVOID lpParam);
HANDLE readThread = NULL;
DWORD readThreadId = 0;

AppData appData;

char rx_buffer[1024];
char tx_buffer[1024];

void app_init(void)
{

}

void app_set_on_new_data(OnNewData nd)
{
    appData.NewDataEvent = nd;
}

void app_set_on_rx_data(OnRxData rx)
{
    appData.RxEvent = rx;
}

void app_set_on_tx_data(OnRxData tx)
{
    appData.TxEvent = tx;
}

uint8_t app_connect(uint8_t port)
{
    uint8_t ret = 0;

    app_disconnect();

    ret = comOpen(port, 9600);
    if (ret > 0)
    {
        appData.Connected = 1;
        appData.PortIndex = port;

        readThread = CreateThread(NULL, 0, ReadThreadFunc, 0, 0, &readThreadId);
    }

    return ret;
}

uint8_t app_get_ports(char ports[][5])
{
    int count = comEnumerate();

    if (count > 0)
    {
        for (int i = 0; i < count; i++)
        {
            strcpy(&ports[i][0], comGetPortName(i));
        }
    }

    return count;
}

void app_disconnect(void)
{
    if (readThread != NULL)
    {
        appData.Connected = 0;
        comCloseAll();
        WaitForSingleObject(&readThread, INFINITE);
        CloseHandle(&readThread);
    }
}

void app_toggle_output(uint8_t output)
{

}

void app_auto_read_io(uint8_t enable)
{

}

void app_write_lcd(char *data, uint8_t line)
{

}

void app_clear_lcd(void)
{

}

DWORD WINAPI ReadThreadFunc(LPVOID lpParam)
{
    int rx_count = 0;
    char *start_pos, *end_pos, *pbuf;

    while (appData.Connected)
    {
        rx_count = comRead(appData.PortIndex, rx_buffer, 1024);

        if (rx_count)
        {
            if (appData.RxEvent)
            {
                printf("1\n");
                appData.RxEvent(rx_buffer, rx_count);

                printf("2\n");
                start_pos = strchr(rx_buffer, START_BYTE);
                end_pos = strchr(rx_buffer, END_BYTE);
                printf("3\n");
                if (end_pos > start_pos)
                {
                    printf("4\n");
                    //if (start_pos > 0)
                    //{
//                        printf("5\n");
//                        pbuf = start_pos - rx_buffer + 1;
//                        switch(pbuf[0])
//                        {
//                        case OUTPUTS_BYTE:
//                            appData.Outputs[0] = pbuf[2] == '1' ? 1 : 0;
//                            appData.Outputs[1] = pbuf[3] == '1' ? 1 : 0;
//                            appData.Outputs[2] = pbuf[4] == '1' ? 1 : 0;
//                            appData.Outputs[3] = pbuf[5] == '1' ? 1 : 0;
//                            break;
//
//                        case INPUTS_BYTE:
//                            break;
//                        }
//
//                        if (appData.NewDataEvent)
//                            appData.NewDataEvent();
                    //}
                }
            }
        }
    }

    return 0;
}
