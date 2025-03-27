#include "config.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include <stdint.h>
#include <string.h>

static const char *TAG = "Config";

// Function to load configuration from NVS or use default values
void load_config_from_nvs(app_config_t *config) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to open NVS. Using default configuration.");
        // Use default values if NVS is not initialized
        strncpy(config->wifi_ssid, "", sizeof(config->wifi_ssid));
        strncpy(config->wifi_password, "", sizeof(config->wifi_password));
        config->servo_pin = DEFAULT_SERVO_PIN;
        config->servo_min_pulse = DEFAULT_SERVO_MIN_PULSE;
        config->servo_max_pulse = DEFAULT_SERVO_MAX_PULSE;
        strncpy(config->tomtom_api_key, "", sizeof(config->tomtom_api_key));
        strncpy(config->latitude, "", sizeof(config->latitude));
        strncpy(config->longitude, "", sizeof(config->longitude));
        strncpy(config->zoom_level, DEFAULT_ZOOM_LEVEL, sizeof(config->zoom_level));
        return;
    }

    size_t len;

    // Load Wi-Fi SSID
    len = sizeof(config->wifi_ssid);
    if (nvs_get_str(nvs_handle, NVS_KEY_WIFI_SSID, config->wifi_ssid, &len) != ESP_OK) {
        ESP_LOGW(TAG, "Wi-Fi SSID not found in NVS. Using default value.");
        strncpy(config->wifi_ssid, "", sizeof(config->wifi_ssid));
    }

    // Load Wi-Fi Password
    len = sizeof(config->wifi_password);
    if (nvs_get_str(nvs_handle, NVS_KEY_WIFI_PASSWORD, config->wifi_password, &len) != ESP_OK) {
        ESP_LOGW(TAG, "Wi-Fi Password not found in NVS. Using default value.");
        strncpy(config->wifi_password, "", sizeof(config->wifi_password));
    }

    // Load Servo Pin
    if (nvs_get_i32(nvs_handle, NVS_KEY_SERVO_PIN, &config->servo_pin) != ESP_OK) {
        ESP_LOGW(TAG, "Servo Pin not found in NVS. Using default value.");
        config->servo_pin = DEFAULT_SERVO_PIN;
    }

    // Load Servo Min Pulse
    if (nvs_get_i32(nvs_handle, NVS_KEY_SERVO_MIN_PULSE, &config->servo_min_pulse) != ESP_OK) {
        ESP_LOGW(TAG, "Servo Min Pulse not found in NVS. Using default value.");
        config->servo_min_pulse = DEFAULT_SERVO_MIN_PULSE;
    }

    // Load Servo Max Pulse
    if (nvs_get_i32(nvs_handle, NVS_KEY_SERVO_MAX_PULSE, &config->servo_max_pulse) != ESP_OK) {
        ESP_LOGW(TAG, "Servo Max Pulse not found in NVS. Using default value.");
        config->servo_max_pulse = DEFAULT_SERVO_MAX_PULSE;
    }

    // Load TomTom API Key
    len = sizeof(config->tomtom_api_key);
    if (nvs_get_str(nvs_handle, NVS_KEY_TOMTOM_API_KEY, config->tomtom_api_key, &len) != ESP_OK) {
        ESP_LOGW(TAG, "TomTom API Key not found in NVS. Using default value.");
        strncpy(config->tomtom_api_key, "", sizeof(config->tomtom_api_key));
    }

    // Load Latitude
    len = sizeof(config->latitude);
    if (nvs_get_str(nvs_handle, NVS_KEY_LATITUDE, config->latitude, &len) != ESP_OK) {
        ESP_LOGW(TAG, "Latitude not found in NVS. Using default value.");
        strncpy(config->latitude, "", sizeof(config->latitude));
    }

    // Load Longitude
    len = sizeof(config->longitude);
    if (nvs_get_str(nvs_handle, NVS_KEY_LONGITUDE, config->longitude, &len) != ESP_OK) {
        ESP_LOGW(TAG, "Longitude not found in NVS. Using default value.");
        strncpy(config->longitude, "", sizeof(config->longitude));
    }

    // Load Zoom Level
    len = sizeof(config->zoom_level);
    if (nvs_get_str(nvs_handle, NVS_KEY_ZOOM_LEVEL, config->zoom_level, &len) != ESP_OK) {
        ESP_LOGW(TAG, "Zoom Level not found in NVS. Using default value.");
        strncpy(config->zoom_level, DEFAULT_ZOOM_LEVEL, sizeof(config->zoom_level));
    }

    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Configuration loaded from NVS.");
}