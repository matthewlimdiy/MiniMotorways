#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"

#define MAX_RETRY 2

// Function declarations
void set_custom_dns();
esp_err_t wifi_init_sta();
esp_err_t wifi_init_ap();

#endif // WIFI_H