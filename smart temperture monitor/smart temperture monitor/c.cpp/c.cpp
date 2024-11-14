#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "dht.h" // Include your DHT library header

#define DHT_PIN GPIO_NUM_4 // GPIO pin where the DHT11 is connected
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASS "YOUR_PASSWORD"

static const char *TAG = "DHT_Sensor";

void wifi_init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}

void app_main() {
    wifi_init();

    dht_init(DHT_PIN); // Initialize the DHT sensor

    while (1) {
        float humidity = 0;
        float temperature = 0;

        // Read temperature and humidity from DHT11
        if (dht_read_data(DHT_TYPE_DHT11, DHT_PIN, &humidity, &temperature) == ESP_OK) {
            ESP_LOGI(TAG, "Humidity: %.1f%%, Temperature: %.1f°C", humidity, temperature);
        } else {
            ESP_LOGE(TAG, "Failed to read from DHT sensor!");
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // Wait for 2 seconds
    }
}