#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

void servo_init(int servo_pin, int min_pulse, int max_pulse);
uint32_t calculate_pulse_width(int angle, int min_pulse, int max_pulse);
void move_servo(int start_angle, int end_angle, int speed, int servo_pin, int min_pulse, int max_pulse);

#endif // SERVO_H