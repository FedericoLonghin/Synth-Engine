#include "comunication_manager.h"
#include "midi_command.h"
const int serialBuff_len = 256;
uint8_t serialBuff[256];
QueueHandle_t cmd_queue_handle;

void command_reciver(void *args)
{
    const char *TAG = "command_reciver";
    uart_config_t uart_config = {
        .baud_rate = SERIAL_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, serialBuff_len, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, SERIAL_TX, SERIAL_RX, GPIO_NUM_NC, GPIO_NUM_NC));

    cmd_queue_handle = xQueueCreate(10, sizeof(command));
    ESP_LOGI(TAG, "uart:%d", uart_enable_rx_intr(UART_PORT_NUM));
    while (1)
    {
        if (uart_read_bytes(UART_PORT_NUM, serialBuff, 8, 10))
        {

            if (serialBuff[0] == MIDI_Note_On || serialBuff[0] == MIDI_Note_Off)
            {
                command cmd =
                    {
                        .cmd = serialBuff[0],
                        .param1 = serialBuff[1],
                        .param2 = 0,
                        .param3 = 0,
                        .val_float = 0};
                xQueueSend(cmd_queue_handle, &cmd, 100);
            }
            else if (serialBuff[0] == MIDI_Set_Env_Param||serialBuff[0] == MIDI_Set_Op_Param)
            {
                ESP_LOGI(TAG, "recived param");
                uint32_t rawVal = (serialBuff[4] << 24 | serialBuff[5] << 16 | serialBuff[6] << 8 | serialBuff[7]);
                float *val = (float *)&rawVal;
                printf("%f\n", *val);
                command cmd = {
                    .cmd = serialBuff[0],
                    .param1 = serialBuff[1],
                    .param2 = serialBuff[2],
                    .param3 = serialBuff[3],
                    .val_float = *val};
                xQueueSend(cmd_queue_handle, &cmd, 100);
            }
        }
        // uart_flush(UART_PORT_NUM);
    }
    vTaskDelete(NULL);
}