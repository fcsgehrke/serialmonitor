#include "app.h"

// Read Thread
DWORD WINAPI ReadThreadFunc(LPVOID lpParam);
HANDLE readThread = NULL;
DWORD readThreadId = 0;

AppData appData;

char rx_buffer[1024];
char tx_buffer[1024];

ring_buffer_t buffer;

void app_init(void)
{
    ring_buffer_init(&buffer);
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
    char frame_buf[24];
    int16_t start_pos, end_pos;

    while (appData.Connected)
    {
        rx_count = comRead(appData.PortIndex, rx_buffer, 1024);

        if (rx_count)
        {
            if (appData.RxEvent)
            {
                appData.RxEvent(rx_buffer, rx_count);
                ring_buffer_queue_arr(&buffer, rx_buffer, rx_count);

                start_pos = ring_buffer_chr(&buffer, START_BYTE);
                if (start_pos >= 0)
                {
                    end_pos = ring_buffer_chr(&buffer, END_BYTE);
                    if (end_pos >= 0)
                    {
                        if (start_pos > 0)
                            ring_buffer_remove(&buffer, start_pos);

                        ring_buffer_dequeue_arr(&buffer, frame_buf, 24);

                        if (frame_buf[2] == IO_READ_BYTE)
                        {
                            appData.Outputs[0] = frame_buf[3] - 0x30;
                            appData.Outputs[1] = frame_buf[4] - 0x30;
                            appData.Outputs[2] = frame_buf[5] - 0x30;
                            appData.Outputs[3] = frame_buf[6] - 0x30;
                            appData.Inputs[0] = frame_buf[7] - 0x30;
                            appData.Inputs[1] = frame_buf[8] - 0x30;
                            appData.Inputs[2] = frame_buf[9] - 0x30;
                            appData.Inputs[3] = frame_buf[10] - 0x30;
                        }

                        if (appData.NewDataEvent)
                            appData.NewDataEvent();
                    }
                }
            }
        }
    }

    return 0;
}
