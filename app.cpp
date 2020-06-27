#include "app.h"

// Read Thread
DWORD WINAPI ReadThreadFunc(LPVOID lpParam);
HANDLE readThread = NULL;
DWORD readThreadId = 0;

AppData appData;

char rx_buffer[1024];
char tx_buffer[1024];

ring_buffer_t buffer;

void create_frame(char *dest, char *data, uint8_t function, uint8_t size)
{
    uint8_t cks = 0;

    dest[0] = START_BYTE;
    dest[1] = function;

    cks += dest[1];

    for (int i = 0; i < size; i++)
    {
        dest[i + 2] = data[i];
        cks += dest[i + 2];
    }

    if ((DATA_SIZE - size) > 0)
    {
        for (int i = size; i < DATA_SIZE; i++)
        {
            dest[i] = 0x30;
            cks += dest[i];
        }
    }

    dest[DATA_SIZE + 2] = cks;
    dest[DATA_SIZE + 3] = END_BYTE;
}

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

void app_write_outputs(uint8_t *values)
{
    char frame[24];
    char outputs[4];

    for (int i = 0; i < 4; i++)
        outputs[i] = values[i] + 0x30;

    create_frame(frame, values, IO_WRITE_BYTE, 4);

    comWrite(appData.PortIndex, frame, 24);
}

void app_read_io(void)
{
    char frame[24];

    create_frame(frame, "", IO_READ_BYTE, 0);

    comWrite(appData.PortIndex, frame, 24);
}

void app_write_lcd(char *data, uint8_t size, uint8_t x, uint8_t y)
{
    char frame[24];
    char lcd_data[20] = "0000                ";

    lcd_data[0] = (x / 10) + 0x30;
    lcd_data[1] = (x % 10) + 0x30;
    lcd_data[2] = 0x30;
    lcd_data[3] = (y / 10) + 0x30;
    strncpy(&lcd_data[4], data, size);
    create_frame(frame, lcd_data, WRITE_LCD_BYTE, 20);

    comWrite(appData.PortIndex, frame, 24);
}

void app_clear_lcd(void)
{
    char frame[24];

    create_frame(frame, "", CLEAR_LCD_BYTE, 0);

    comWrite(appData.PortIndex, frame, 24);
}

void app_set_pwm(uint8_t value)
{
    char frame[24];
    char pwm_data[3];

    pwm_data[0] = (value / 100);
    value -=  pwm_data[0];

    pwm_data[0] += 0x30;
    pwm_data[1] = (value / 10) + 0x30;
    pwm_data[2] = (value % 10) + 0x30;

    create_frame(frame, pwm_data, WRITE_PWM_BYTE, 3);

    comWrite(appData.PortIndex, frame, 24);
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
