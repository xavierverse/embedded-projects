#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "DHT.h"  // Include the library for your sensor, e.g., DHT sensor
#include "esp_http_client.h"

// Replace these with your actual values
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_wifi_password"
#define WEATHER_SERVER_IP "192.168.137.129"
#define WEATHER_SERVER_PORT 1234
#define WEB_PATH "/weather"
#define WEB_SERVER "example.com"  // Replace with your server address

static const char *TAG = "weather_station";

SemaphoreHandle_t wifi_semaphore;

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event) {
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xSemaphoreGive(wifi_semaphore);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            esp_wifi_connect();
            break;
        default:
            break;
    }
    return ESP_OK;
}

static void wifi_init_sta() {
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
}

static void http_post_task(void *pvParameters) {
    float temperature, humidity;

    while (1) {
        if (xSemaphoreTake(wifi_semaphore, portMAX_DELAY)) {
            // Read sensor data
            read_sensor_data(&temperature, &humidity);

            // Construct post data
            char post_data[64];  // Adjust the size based on your needs
            snprintf(post_data, sizeof(post_data), "temperature=%.2f&humidity=%.2f", temperature, humidity);

            // Construct HTTP request
            char request[256];  // Adjust the size based on your needs
            snprintf(request, sizeof(request),
                     "POST " WEB_PATH " HTTP/1.0\r\n"
                     "Host: " WEB_SERVER ":" STRINGIZE(WEATHER_SERVER_PORT) "\r\n"
                     "User-Agent: esp-idf/1.0 esp32\r\n"
                     "\r\n%s", post_data);

            // Perform HTTP POST
            esp_http_client_config_t config = {
                .url = NULL,
                .host = WEB_SERVER,
                .port = WEATHER_SERVER_PORT,
                .path = WEB_PATH,
                .method = HTTP_METHOD_POST,
                .user_data = post_data,
                .user_data_len = strlen(post_data),
            };

            esp_http_client_handle_t client = esp_http_client_init(&config);
            esp_err_t err = esp_http_client_perform(client);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "HTTP POST Status = %d", esp_http_client_get_status_code(client));
            } else {
                ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
            }

            esp_http_client_cleanup(client);
            vTaskDelay(pdMS_TO_TICKS(3600000));  // Sleep for 1 hour before the next reading
        }
    }
}

void app_main() {
    nvs_flash_init();
    wifi_semaphore = xSemaphoreCreateBinary();

    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));

    wifi_init_sta();

    xTaskCreate(http_post_task, "http_post_task", 4096, NULL, 5, NULL);
}

