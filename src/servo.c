#include "servo.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "Servo";

void servo_init(int servo_pin, int min_pulse, int max_pulse) {
    ESP_LOGI(TAG, "Initializing servo on pin %d with min_pulse %d and max_pulse %d", servo_pin, min_pulse, max_pulse);

    // Configure the LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_14_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_LOGI(TAG, "Configuring LEDC timer...");
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ESP_LOGI(TAG, "LEDC timer configured.");

    // Configure the LEDC channel
    ledc_channel_config_t ledc_channel = {
        .gpio_num = servo_pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_LOGI(TAG, "Configuring LEDC channel...");
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ESP_LOGI(TAG, "LEDC channel configured.");
    ESP_LOGI(TAG, "Servo initialization complete.");
}

uint32_t calculate_pulse_width(int angle, int min_pulse, int max_pulse) {
    // Ensure the angle is within the valid range (-90 to 90 degrees)
    if (angle < -90) {
        angle = -90;
    } else if (angle > 90) {
        angle = 90;
    }

    return min_pulse + ((angle + 90) * (max_pulse - min_pulse)) / 180;
}

void move_servo(int start_angle, int end_angle, int speed, int servo_pin, int min_pulse, int max_pulse) {
    if (speed < 1) {
        speed = 1;
    } else if (speed > 10) {
        speed = 10;
    }

    int step = (start_angle < end_angle) ? 1 : -1;
    int delay = (11 - speed) * 10; // Delay in milliseconds

    for (int angle = start_angle; angle != end_angle + step; angle += step) {
        uint32_t pulse_width = calculate_pulse_width(angle, min_pulse, max_pulse);

        // Convert pulse width to duty cycle for LEDC
        uint32_t duty_cycle = (pulse_width * (1 << 14)) / 20000; // 14-bit resolution, 20ms period
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty_cycle);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

        vTaskDelay(pdMS_TO_TICKS(delay));
    }
}
