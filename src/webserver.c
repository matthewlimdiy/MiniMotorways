#include "webserver.h"
#include "config.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "WebServer";

static esp_err_t get_handler(httpd_req_t *req) {
    const char *html_form = "<!DOCTYPE html>"
                            "<html>"
                            "<head><title>MiniMotorways Configuration</title></head>"
                            "<body>"
                            "<h1>MiniMotorways Configuration</h1>"
                            "<form action=\"/save\" method=\"POST\">"
                            "<fieldset>"
                            "<legend><b>Wi-Fi Configuration</b></legend>"
                            "Wi-Fi SSID:<br><input type=\"text\" name=\"wifi_ssid\" placeholder=\"Enter SSID\"><br><br>"
                            "Wi-Fi Password:<br><input type=\"password\" name=\"wifi_password\" placeholder=\"Enter Password\"><br><br>"
                            "</fieldset><br>"
                            "<fieldset>"
                            "<legend><b>Servo Configuration</b></legend>"
                            "Servo Pin:<br><input type=\"number\" name=\"servo_pin\" placeholder=\"Enter Servo Pin\"><br><br>"
                            "Servo Min Pulse:<br><input type=\"number\" name=\"servo_min_pulse\" placeholder=\"Enter Min Pulse\"><br><br>"
                            "Servo Max Pulse:<br><input type=\"number\" name=\"servo_max_pulse\" placeholder=\"Enter Max Pulse\"><br><br>"
                            "</fieldset><br>"
                            "<fieldset>"
                            "<legend><b>TomTom API Configuration</b></legend>"
                            "TomTom API Key:<br><input type=\"text\" name=\"tomtom_api_key\" placeholder=\"Enter API Key\"><br><br>"
                            "Latitude:<br><input type=\"text\" name=\"latitude\" placeholder=\"Enter Latitude\"><br><br>"
                            "Longitude:<br><input type=\"text\" name=\"longitude\" placeholder=\"Enter Longitude\"><br><br>"
                            "Zoom Level:<br><input type=\"text\" name=\"zoom_level\" placeholder=\"Enter Zoom Level\"><br><br>"
                            "</fieldset><br>"
                            "<input type=\"submit\" value=\"Save Configuration\">"
                            "</form>"
                            "</body>"
                            "</html>";
    httpd_resp_send(req, html_form, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t post_handler(httpd_req_t *req) {
    char buf[512];
    ESP_LOGI(TAG, "Received request with content length: %d", req->content_len);
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) continue;
            return ESP_FAIL;
        }
        remaining -= ret;
    }

    buf[req->content_len] = '\0';

    // Parse the form data
    char wifi_ssid[32] = {0}, wifi_password[64] = {0};
    int servo_pin = 0, servo_min_pulse = 0, servo_max_pulse = 0;
    char tomtom_api_key[64] = {0}, latitude[16] = {0}, longitude[16] = {0}, zoom_level[8] = {0};

    sscanf(buf,
           "wifi_ssid=%31[^&]&wifi_password=%63[^&]&servo_pin=%d&servo_min_pulse=%d&servo_max_pulse=%d"
           "&tomtom_api_key=%63[^&]&latitude=%15[^&]&longitude=%15[^&]&zoom_level=%7s",
           wifi_ssid, wifi_password, &servo_pin, &servo_min_pulse, &servo_max_pulse,
           tomtom_api_key, latitude, longitude, zoom_level);

    ESP_LOGI(TAG, "Received Configuration:");
    ESP_LOGI(TAG, "Wi-Fi SSID: %s, Password: %s", wifi_ssid, wifi_password);
    ESP_LOGI(TAG, "Servo Pin: %d, Min Pulse: %d, Max Pulse: %d", servo_pin, servo_min_pulse, servo_max_pulse);
    ESP_LOGI(TAG, "TomTom API Key: %s, Latitude: %s, Longitude: %s, Zoom Level: %s",
             tomtom_api_key, latitude, longitude, zoom_level);

    // Save to NVS
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, NVS_KEY_WIFI_SSID, wifi_ssid));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, NVS_KEY_WIFI_PASSWORD, wifi_password));
    ESP_ERROR_CHECK(nvs_set_i32(nvs_handle, NVS_KEY_SERVO_PIN, servo_pin));
    ESP_ERROR_CHECK(nvs_set_i32(nvs_handle, NVS_KEY_SERVO_MIN_PULSE, servo_min_pulse));
    ESP_ERROR_CHECK(nvs_set_i32(nvs_handle, NVS_KEY_SERVO_MAX_PULSE, servo_max_pulse));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, NVS_KEY_TOMTOM_API_KEY, tomtom_api_key));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, NVS_KEY_LATITUDE, latitude));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, NVS_KEY_LONGITUDE, longitude));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, NVS_KEY_ZOOM_LEVEL, zoom_level));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);

    httpd_resp_send(req, "Configuration saved! Rebooting...", HTTPD_RESP_USE_STRLEN);

    // Reboot the device
    esp_restart();
    return ESP_OK;
}

void start_webserver() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t get_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = get_handler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(server, &get_uri);

        httpd_uri_t post_uri = {
            .uri = "/save",
            .method = HTTP_POST,
            .handler = post_handler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(server, &post_uri);

        ESP_LOGI(TAG, "Web server started.");
    } else {
        ESP_LOGE(TAG, "Failed to start web server.");
    }
}