#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c.h"

#define ICM42670_I2C_ADDR 0x68
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_SDA_GPIO_NUM 10
#define I2C_SCL_GPIO_NUM 8

#define ICM42670_I2C_ADDR 0x68

static const char *TAG = "ICM42670";

int16_t twosCompToDec(int16_t two_compliment_val)
{
    // [0x0000; 0x7FFF] corresponds to [0; 32,767]
    // [0x8000; 0xFFFF] corresponds to [-32,768; -1]
    // int16_t has the range [-32,768; 32,767]

    uint16_t sign_mask = 0x8000;

    // if positive
    if ( (two_compliment_val & sign_mask) == 0 ) {
        return two_compliment_val;
    //  if negative
    } else {
        // invert all bits, add one, and make negative
        return -(~two_compliment_val + 1);
    }
}

void i2c_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_GPIO_NUM,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_GPIO_NUM,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}

void icm42670_init() {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ICM42670_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x1F, 1);  // Set configuration registers as needed
    i2c_master_write_byte(cmd, 0x0F, 1);  // Set configuration registers as needed
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

int16_t read_accelerometer_data(uint8_t register_address) {
    uint8_t data[2];

    // Read accelerometer data from the ICM-42670-P
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ICM42670_I2C_ADDR << 1) | I2C_MASTER_WRITE, 1);
    i2c_master_write_byte(cmd, register_address, 1);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ICM42670_I2C_ADDR << 1) | I2C_MASTER_READ, 1);
    i2c_master_read(cmd, data, 2, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    // Combine the high and low bytes for the accelerometer data
    int16_t acceleration_raw = (int16_t)((data[0] << 8) | data[1]);
    int16_t acceleration = twosCompToDec(acceleration_raw);
    return acceleration;
}

void app_main() {
    i2c_init();
    icm42670_init();
    
    while (1) {
        int16_t x_acceleration = read_accelerometer_data(0x0B);
        //int16_t x_acceleration_lsb = read_accelerometer_data(0x0C);
        int16_t y_acceleration = read_accelerometer_data(0x0D);
        //int16_t y_acceleration_lsb = read_accelerometer_data(0x0E);
	// int16_t z_acceleration = read_accelerometer_data(0x0F);

	if ((x_acceleration < -500) && (y_acceleration > 500)) { // RIGHT UP
	    ESP_LOGI(TAG, "UP RIGHT");
	} else if ((x_acceleration < -500) && (y_acceleration < -500)) { // DOWN RIGHT	    
	    ESP_LOGI(TAG, "DOWN RIGHT");
	} else if ((x_acceleration > 500) && (y_acceleration > 500)) { // left up
		ESP_LOGI(TAG, "UP LEFT");
	} else if ((x_acceleration > 500) && (y_acceleration < -500)) { // left down
		ESP_LOGI(TAG, "DOWN LEFT");
	} else if ((x_acceleration < -500) && (y_acceleration > -500) && (y_acceleration < 500)) { // right
	    ESP_LOGI(TAG, "RIGHT");
	} else if ((x_acceleration >500) && (y_acceleration > -500) && (y_acceleration < 500)) { // left
	    ESP_LOGI(TAG, "LEFT");
	} else if ((y_acceleration > 500) && (x_acceleration > -500) && (x_acceleration < 500)) { // up
	    ESP_LOGI(TAG, "UP");
	} else if ((y_acceleration < -500) && (x_acceleration > -500) && (x_acceleration < 500)) { // down
	    ESP_LOGI(TAG, "DOWN");
	} else {
	    ESP_LOGI(TAG, "NO DIRECTION");
	}
	//ESP_LOGI(TAG, "X-axis Acceleration: %d", x_acceleration);
        //ESP_LOGI(TAG, "Y-axis Acceleration: %d", y_acceleration);
        // ESP_LOGI(TAG, "Z-axis Acceleration: %d", z_acceleration);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Read data every 1 second
    }
}
