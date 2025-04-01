#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/temp_sensor.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "esp_system.h"
#include "driver/i2c.h"

// SHTC3 I2C address
#define SHTC3_I2C_ADDR 0x70

// SHTC3 commands
#define SHTC3_MEASURE_CMD 0x7C

#define I2C_MASTER_SCL_IO 8
#define I2C_MASTER_SDA_IO 10
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_FREQ 400000
#define TRIGGER_PIN GPIO_NUM_0
#define ECHO_PIN GPIO_NUM_1

static const char *TAG = "US-100";

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

float get_temperature()
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

    int temperature = (data[0] << 8 | data[1]);
    float temperature_degC = -45 + 175 * (temperature / 65535.0);
    return temperature_degC;
}

void configure_gpio() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << TRIGGER_PIN),
        .mode = GPIO_MODE_OUTPUT, // ECHO pin is an input
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_config_t io_conf2 = {
        .pin_bit_mask = (1ULL << ECHO_PIN),
        .mode = GPIO_MODE_INPUT, // ECHO pin is an input
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf2);
}



int64_t measure_pulse_duration() {

    gpio_set_level(TRIGGER_PIN, 1); // Trigger the sensor
    esp_rom_delay_us(10); // Wait for the sensor to stabilize
    gpio_set_level(TRIGGER_PIN, 0); // Release the trigger

    while (gpio_get_level(ECHO_PIN) == 0) {}
    int64_t start = esp_timer_get_time();

    while (gpio_get_level(ECHO_PIN) == 1){}
    int64_t end = esp_timer_get_time(); 
    
    int64_t diff = end - start;
    return diff;
}


void app_main() {
    i2c_master_init();
    configure_gpio();
    esp_timer_early_init();
    while (1) {
	float pulse = measure_pulse_duration();
	float speed = (331 + (0.6 * get_temperature())) / 20000;
        
	
	float distance = speed * pulse;
        
	ESP_LOGI(TAG, "Distance: %.2fcm at %.2f", distance, get_temperature());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

