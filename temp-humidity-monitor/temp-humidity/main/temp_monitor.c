#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

// SHTC3 I2C address
#define SHTC3_I2C_ADDR 0x70

// SHTC3 commands
#define SHTC3_MEASURE_CMD 0x7C

#define I2C_MASTER_SCL_IO 8
#define I2C_MASTER_SDA_IO 10
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_FREQ 400000


#define CONFIG_SCL_PIN GPIO_NUM_11
static const char *TAG = "SHTC3";

esp_err_t i2c_master_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,  // Define your SDA pin
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,  // Define your SCL pin
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ,  // I2C speed
    };

    i2c_param_config(I2C_MASTER_NUM, &conf);
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void i2c_read_data()
{
    uint8_t cmd[2] = { 0x78, 0x66 };
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHTC3_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd_handle, cmd, 2, true);
    i2c_master_stop(cmd_handle);
    i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_handle);

    vTaskDelay(20 / portTICK_PERIOD_MS);

    uint8_t data[6];        // Trigger a measurement
    cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHTC3_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd_handle, data, 6, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd_handle);
    i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_handle);

    // vTaskDelay(pdMS_TO_TICKS(20));  // Wait for the measurement to complete (adjust as needed)

    // Read the measurement data
    int temperature = (data[0] << 8 | data[1]);
    int humidity = (data[3] << 8 | data[4]);

    float temperature_degC = -45 + 175 * (temperature / 65535.0);
    float temperature_degF = temperature_degC * (9 / 5) + 32;
    float humidity_percent = 100 * (humidity / 65535.0);

    ESP_LOGI(TAG, "Temperature is %.2fC (or %.2fF) with a %.2f %% humidity\n", temperature_degC, temperature_degF, humidity_percent);

    // vTaskDelay(pdMS_TO_TICKS(2000));  // Update every 2 seconds
}

void app_main()
{
    ESP_ERROR_CHECK(i2c_master_init());
    while (1) {
    	i2c_read_data();
    	vTaskDelay(2000 / portTICK_PERIOD_MS);  // Update every 2 seconds
    }
}
