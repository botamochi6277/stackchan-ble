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

  servo_pan.setPeriodHertz(50);  // standard 50 hz servo
  // servo_pan.attach(32);
  servo_tilt.setPeriodHertz(50);  // standard 50 hz servo
  // servo_tilt.attach(33);

  // // set idle position
  // servo_pan.write(90);
  // servo_tilt.write(90);

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

  if (stackchan_srv.is_servo_activated_chr.written()) {
    if (stackchan_srv.is_servo_activated_chr.value()) {
      // activate servo
      servo_pan.attach(32);
      servo_tilt.attach(33);
    } else {
      servo_pan.detach();
      servo_tilt.detach();
    }
  }

  if (stackchan_srv.is_servo_activated_chr.value()) {
    if (stackchan_srv.servo_pan_angle_chr.written()) {
      auto angle = stackchan_srv.servo_pan_angle_chr.value();
      if (angle < pan_min) {
        angle = pan_min;
      }
      if (pan_max < angle) {
        angle = pan_max;
      }
      servo_pan.write(angle);
    }
    if (stackchan_srv.servo_tilt_angle_chr.written()) {
      auto angle = stackchan_srv.servo_tilt_angle_chr.value();
      if (angle < tilt_min) {
        angle = tilt_min;
      }
      if (tilt_max < angle) {
        angle = tilt_max;
      }
      servo_tilt.write(angle);
    }
  }

  // time_sec = millis() * 1.0e-3f;

  // sweepUpdate(servo_pan, time_sec, 0.1f, 30.0f);
  // sweepUpdate(servo_tilt, time_sec, 0.1f, 10.0f, 90.0f);
  delay(10);
}