#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "connection.h"

unsigned long motionStartTime = 0;
unsigned long lastMotionDetectedTime = 0;
bool motionDetected = false;
bool longMotionStartedPublishing = false;

#define OFF_TIME 3000                  // Time to consider motion as ended after sensor goes LOW
#define LONG_MOTION_TIME 6000          // Motion detected for more than 3 seconds is considered long motion
#define DURATION_PUBLISH_INTERVAL 1000 // Publish duration at most every 1 second
unsigned long lastDurationPublishTime = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Turn off the LED
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  pinMode(SENSOR_PIN, INPUT);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  int motionState = digitalRead(SENSOR_PIN);
  unsigned long currentTime = millis();

  if (motionState == HIGH && !motionDetected)
  {
    motionDetected = true;
    motionStartTime = currentTime;
    Serial.println("Motion detected");
    client.publish(STATE_TOPIC, "true");
    digitalWrite(LED_BUILTIN, LOW); // Turn on the LED
  }
  else if (motionState == LOW && motionDetected && currentTime - lastMotionDetectedTime > OFF_TIME)
  {
    motionDetected = false;
    longMotionStartedPublishing = false;
    Serial.println("Motion ended");
    client.publish(STATE_TOPIC, "false");
    digitalWrite(LED_BUILTIN, HIGH); // Turn off the LED
  }

  if (motionDetected && currentTime - motionStartTime > LONG_MOTION_TIME)
  {
    if (!longMotionStartedPublishing)
    {
      longMotionStartedPublishing = true;
      Serial.println("Long motion detected");
    }

    // Publish duration every DURATION_PUBLISH_INTERVAL
    if (currentTime - lastDurationPublishTime > DURATION_PUBLISH_INTERVAL)
    {
      int duration = (currentTime - motionStartTime) / 1000;
      char durationMsg[10];
      sprintf(durationMsg, "%d", duration);
      Serial.println("Long motion duration: " + String(durationMsg) + " seconds");
      client.publish(DURATION_TOPIC, durationMsg);
      lastDurationPublishTime = currentTime;
    }
  }

  if (motionState == HIGH)
  {
    lastMotionDetectedTime = currentTime;
  }
  delay(100);
}