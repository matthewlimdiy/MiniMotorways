#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define DEFAULT_SERVO_PIN 2
#define DEFAULT_SERVO_MIN_PULSE 510
#define DEFAULT_SERVO_MAX_PULSE 2490
#define DEFAULT_ZOOM_LEVEL "14"
#define DEFAULT_SERVO_SPEED 6

// NVS Keys for Configuration
#define NVS_KEY_WIFI_SSID "wifi_ssid"
#define NVS_KEY_WIFI_PASSWORD "wifi_password"
#define NVS_KEY_SERVO_PIN "servo_pin"
#define NVS_KEY_SERVO_MIN_PULSE "servo_min_pulse"
#define NVS_KEY_SERVO_MAX_PULSE "servo_max_pulse"
#define NVS_KEY_TOMTOM_API_KEY "tomtom_api_key"
#define NVS_KEY_LATITUDE "latitude"
#define NVS_KEY_LONGITUDE "longitude"
#define NVS_KEY_ZOOM_LEVEL "zoom_level"

// Default AP Mode Configuration
#define AP_SSID "MiniMotorways"
#define AP_PASSWORD "minimotorways"
#define AP_CHANNEL 1
#define AP_MAX_CONN 4

// Configuration structure
typedef struct {
    char wifi_ssid[32];
    char wifi_password[64];
    int32_t servo_pin;
    int32_t servo_min_pulse;
    int32_t servo_max_pulse;
    char tomtom_api_key[64];
    char latitude[16];
    char longitude[16];
    char zoom_level[8];
} app_config_t;

void load_config_from_nvs(app_config_t *config);

#endif // CONFIG_H