#include "i2s_manager.h"

i2s_chan_handle_t tx_handle;
i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
i2s_std_config_t std_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
    .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
    .gpio_cfg = {
        .mclk = I2S_GPIO_UNUSED,
        .bclk = GPIO_NUM_4,
        .ws = GPIO_NUM_5,
        .dout = GPIO_NUM_18,
        .din = I2S_GPIO_UNUSED,
        .invert_flags = {
            .mclk_inv = false,
            .bclk_inv = false,
            .ws_inv = false,
        },
    },
};

void i2s_init()
{
    const char *TAG = "i2s_init";
    esp_err_t err = ESP_OK;
    err = i2s_new_channel(&chan_cfg, &tx_handle, NULL);

    err = i2s_channel_init_std_mode(tx_handle, &std_cfg);
    if (err != ESP_OK)
        ESP_LOGE(TAG, "Err initialising new channel");
    else
        ESP_LOGI(TAG, "New channel initialized");

    xTaskCreate(i2s_write_task, "i2s_write_task", 10000, NULL, 5, NULL);
}

int16_t outBuff_A[outBuff_size];
int16_t outBuff_B[outBuff_size];
int16_t *outBuffer_toFill;
bool fillBufferREQ = false;
bool bufferFilled = false;

void i2s_write_task(void *args)
{
    const char *TAG = "i2s_write_task";

    for (int i = 0; i < outBuff_size; i++)
    {
        outBuff_A[i] = outBuff_B[i] = 0;
    }

    esp_err_t err = i2s_channel_enable(tx_handle);

    if (err != ESP_OK)
        ESP_LOGE(TAG, "Err enabling new channel");
    else
        ESP_LOGI(TAG, "New channel enabled");

    int i = 0;
    uint8_t bytes_written = 0;

//initial
    outBuffer_toFill = &outBuff_A;
    fillBufferREQ=true;

    while (1)
    {
        if (outBuffer_toFill == &outBuff_A)
        {
            while (!bufferFilled)
            {
                vTaskDelay(1);
            }
            outBuffer_toFill = &outBuff_B;
            fillBufferREQ = true;
            err = i2s_channel_write(tx_handle, outBuff_A, outBuff_size, &bytes_written, portMAX_DELAY);
            ESP_LOGI(TAG, "writing buff A");
        }
        else if (outBuffer_toFill == &outBuff_B)
        {

            while (!bufferFilled)
            {
                vTaskDelay(1);
            }
            outBuffer_toFill = &outBuff_A;
            fillBufferREQ = true;
            err = i2s_channel_write(tx_handle, outBuff_B, outBuff_size, &bytes_written, portMAX_DELAY);
            ESP_LOGI(TAG, "writing buff B");
        }
        else
        {
            ESP_LOGE(TAG, "outBuffer_toFill val not recognized!");
        }

        i++;
        // printf("%d\n", i);
        vTaskDelay(1);
    }
    vTaskDelete(NULL);
}