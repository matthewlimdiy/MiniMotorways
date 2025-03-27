#ifndef TRAFFIC_H
#define TRAFFIC_H

// Function to get traffic condition using dynamic API key and location
float get_traffic_condition(const char *zoom_level, const char *api_key, const char *latitude, const char *longitude);

// Function to scale speed based on traffic severity
int scale_speed(float traffic_severity);

#endif // TRAFFIC_H
