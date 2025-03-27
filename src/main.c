#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "wifi.h"
#include "servo.h"
#include "traffic.h"
#include "webserver.h"
#include "config.h"
#include "nvs_flash.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "esp_http_client.h"


// Global variables
static const char *TAG = "TrafficApp";
int speed = DEFAULT_SERVO_SPEED;

// Task to update traffic data every minute
void traffic_update_task(void *param) {
    if (param == NULL) {
        ESP_LOGE("TrafficUpdateTask", "Configuration parameter is NULL. Exiting task.");
        vTaskDelete(NULL);
        return;
    }

    app_config_t *config = (app_config_t *)param;

    while (1) {
        ESP_LOGI("TrafficUpdateTask", "Fetching traffic data...");
        float traffic_severity = get_traffic_condition(config->zoom_level, config->tomtom_api_key, config->latitude, config->longitude);

        speed = scale_speed(traffic_severity);

        if (traffic_severity >= 0.0f) {
            ESP_LOGI("TrafficUpdateTask", "Traffic severity: %.2f, Servo speed: %d", traffic_severity, speed);
        } else {
            ESP_LOGW("TrafficUpdateTask", "Failed to fetch traffic data. Using default speed.");
        }

        ESP_LOGI("TrafficUpdateTask", "Traffic data fetched. Waiting for next update...");
        vTaskDelay(pdMS_TO_TICKS(60000)); // Wait 1 minute
    }
}

// Main application entry point
void app_main() {
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "Starting application...");

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Load configuration from NVS
    app_config_t config;
    load_config_from_nvs(&config);

    // Log the loaded configuration
    ESP_LOGI(TAG, "Loaded Configuration:");
    ESP_LOGI(TAG, "Wi-Fi SSID: %s", config.wifi_ssid);
    ESP_LOGI(TAG, "Servo Pin: %ld, Min Pulse: %ld, Max Pulse: %ld", config.servo_pin, config.servo_min_pulse, config.servo_max_pulse);
    ESP_LOGI(TAG, "TomTom API Key: %s", config.tomtom_api_key);
    ESP_LOGI(TAG, "Latitude: %s, Longitude: %s, Zoom Level: %s", config.latitude, config.longitude, config.zoom_level);

    // Initialize the servo with configuration from config.h
    servo_init(config.servo_pin, config.servo_min_pulse, config.servo_max_pulse);

    // Initialize Wi-Fi
    if (wifi_init_sta() != ESP_OK) {
        ESP_LOGW(TAG, "Failed to connect to Wi-Fi. Starting in AP mode...");
        wifi_init_ap();
        start_webserver();
    } else {
        ESP_LOGI(TAG, "Connected to Wi-Fi.");

        // Fetch traffic data once and move servo if successful
        float traffic_severity = get_traffic_condition(config.zoom_level, config.tomtom_api_key, config.latitude, config.longitude);
        if (traffic_severity >= 0.0f) {
            ESP_LOGI(TAG, "Initial traffic data fetched successfully. Moving servo 30 degrees twice...");
            move_servo(30, 60, 8, config.servo_pin, config.servo_min_pulse, config.servo_max_pulse);
            vTaskDelay(pdMS_TO_TICKS(500));
            move_servo(60, 90, 8, config.servo_pin, config.servo_min_pulse, config.servo_max_pulse);
            vTaskDelay(pdMS_TO_TICKS(3000));
        } else {
            ESP_LOGW(TAG, "Failed to fetch initial traffic data.");
        }
    }

    // Create a task to update traffic data every minute
    xTaskCreate(traffic_update_task, "TrafficUpdateTask", 10240, &config, 1, NULL);

    // Main loop to continuously move the servo
    while (1) {
        ESP_LOGI(TAG, "Moving servo 180 degrees and back...");
        move_servo(90, -90, speed, config.servo_pin, config.servo_min_pulse, config.servo_max_pulse);
        move_servo(-90, 90, speed, config.servo_pin, config.servo_min_pulse, config.servo_max_pulse);

        vTaskDelay(pdMS_TO_TICKS(100)); // Add a small delay
    }
}
