#include "traffic.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>
#include "config.h"

static const char *TAG = "Traffic";

float get_traffic_condition(const char *zoom_level, const char *api_key, const char *latitude, const char *longitude) {
    char url[256];
    snprintf(url, sizeof(url),
             "http://api.tomtom.com/traffic/services/4/flowSegmentData/absolute/%s/json?key=%s&point=%s,%s",
             zoom_level, api_key, latitude, longitude);

    ESP_LOGI(TAG, "Requesting traffic data from URL: %s", url);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .buffer_size = 512,
        .timeout_ms = 20000,
        .transport_type = HTTP_TRANSPORT_OVER_TCP,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "User-Agent", "ESP32");
    esp_http_client_set_header(client, "Accept", "application/json");

    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return -1.0f;
    }

    int content_length = esp_http_client_fetch_headers(client);
    if (content_length <= 0) {
        ESP_LOGE(TAG, "Invalid content length: %d", content_length);
        esp_http_client_cleanup(client);
        return -1.0f;
    }

    char *response = malloc(content_length + 1);
    if (!response) {
        ESP_LOGE(TAG, "Failed to allocate memory for response.");
        esp_http_client_cleanup(client);
        return -1.0f;
    }
    memset(response, 0, content_length + 1);

    int read_len = esp_http_client_read(client, response, content_length);
    if (read_len <= 0) {
        ESP_LOGE(TAG, "Failed to read response. Read length: %d", read_len);
        free(response);
        esp_http_client_cleanup(client);
        return -1.0f;
    }

    response[read_len] = '\0';
    ESP_LOGI(TAG, "Response: %s", response);

    cJSON *json = cJSON_Parse(response);
    free(response);
    if (!json) {
        ESP_LOGE(TAG, "Failed to parse JSON.");
        esp_http_client_cleanup(client);
        return -1.0f;
    }

    cJSON *flowSegmentData = cJSON_GetObjectItem(json, "flowSegmentData");
    if (flowSegmentData) {
        cJSON *currentSpeed = cJSON_GetObjectItem(flowSegmentData, "currentSpeed");
        cJSON *freeFlowSpeed = cJSON_GetObjectItem(flowSegmentData, "freeFlowSpeed");

        if (currentSpeed && freeFlowSpeed && cJSON_IsNumber(currentSpeed) && cJSON_IsNumber(freeFlowSpeed)) {
            float traffic_severity = 1.0f - ((float)currentSpeed->valuedouble / (float)freeFlowSpeed->valuedouble);
            cJSON_Delete(json);
            esp_http_client_cleanup(client);
            return traffic_severity;
        }
    }

    cJSON_Delete(json);
    esp_http_client_cleanup(client);
    return -1.0f;
}

int scale_speed(float traffic_severity) {
    if (traffic_severity < 0.0f) {
        ESP_LOGW(TAG, "Error fetching traffic data. Using default speed.");
        return DEFAULT_SERVO_SPEED;
    }
    return (int)(10 - (traffic_severity * 9));
}
