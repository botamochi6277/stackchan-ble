#include <ArduinoBLE.h>
#include <Avatar.h>
#include <ESP32Servo.h>
#include <M5Unified.h>

#include "BLEStackchanService.hpp"

m5avatar::Avatar avatar;
ble::StackchanService stackchan_srv;
Servo servo_pan;
Servo servo_tilt;
uint8_t pan_min = 60;
uint8_t pan_max = 120;
uint8_t tilt_min = 80;
uint8_t tilt_max = 100;

float time_sec = 0.0f;
unsigned long milli_sec = 0U;

const m5avatar::Expression expressions[] = {
    m5avatar::Expression::Neutral, m5avatar::Expression::Happy,
    m5avatar::Expression::Angry,   m5avatar::Expression::Sad,
    m5avatar::Expression::Sleepy,  m5avatar::Expression::Doubt};
const uint8_t expressions_size = 6;
m5avatar::Expression current_expression = expressions[0];

void sweepUpdate(Servo& servo, float t, float f, float amp = 90.0f,
                 int offset = 90) {
  int angle = static_cast<int>(amp * sin(2.0f * M_PI * f * t)) + offset;
  servo.write(angle);
}

void setup() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servo_pan.setPeriodHertz(50);   // standard 50 hz servo
  servo_tilt.setPeriodHertz(50);  // standard 50 hz servo

  stackchan_srv.pan_limit.min = 60;
  stackchan_srv.pan_limit.max = 90;
  stackchan_srv.tilt_limit.min = 80;
  stackchan_srv.tilt_limit.max = 100;

  M5.begin();
  avatar.init();  // start drawing

  if (!BLE.begin()) {
    // "starting BLE failed!"
  }

  String ble_address = BLE.address();
  String local_name = "Stackchan_" + ble_address;
  BLE.setDeviceName(local_name.c_str());
  BLE.setLocalName("Stackchan");
  BLE.setAdvertisedService(stackchan_srv);

  // add service
  BLE.addService(stackchan_srv);
  stackchan_srv.setInitialValues();
  // start advertising
  BLE.advertise();
}

void loop() {
  // avatar's face updates in another thread
  // so no need to loop-by-loop rendering

  milli_sec = millis();
  stackchan_srv.timer_chr.writeValue(milli_sec);
  BLE.poll();

  stackchan_srv.servoPoll(servo_pan, servo_tilt);
  stackchan_srv.facePoll(avatar, expressions, expressions_size);

  // time_sec = millis() * 1.0e-3f;

  delay(10);
}