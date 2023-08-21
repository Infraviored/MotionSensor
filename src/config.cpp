#include "config.h"
#include <ESP8266WiFi.h>

//! IMPORTANT! RENAME!
const char *MQTT_DEVICE_NAME = "Bewegungsmelder_Radar";
const char *STATE_TOPIC = "Motion/Bad/state";
const char *DURATION_TOPIC = "Motion/Bad/duration";

// Pin to which the sensor is connected
const int SENSOR_PIN = D1;

