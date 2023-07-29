#include <Avatar.h>
#include <ESP32Servo.h>
#include <M5Unified.h>

m5avatar::Avatar avatar;

Servo servo_yaw;
Servo servo_pitch;

float time_sec = 0.0f;

void sweepUpdate(Servo& servo, float t, float f, float amp = 90.0f,
                 int offset = 90) {
  int angle = static_cast<int>(amp * sin(2.0f * M_PI * f * t)) + offset;
  servo.write(angle);
}

void setup() {
  // ESP32PWM::allocateTimer(0);
  // ESP32PWM::allocateTimer(1);
  // ESP32PWM::allocateTimer(2);
  // ESP32PWM::allocateTimer(3);

  // servo_yaw.setPeriodHertz(50);  // standard 50 hz servo
  // servo_yaw.attach(32);
  // servo_pitch.setPeriodHertz(50);  // standard 50 hz servo
  // servo_pitch.attach(33);

  // // set idle position
  // servo_yaw.write(90);
  // servo_pitch.write(90);

  M5.begin();
  avatar.init();  // start drawing

  // delay(1000);  // wait for servo
}

void loop() {
  // avatar's face updates in another thread
  // so no need to loop-by-loop rendering

  // time_sec = millis() * 1.0e-3f;

  // sweepUpdate(servo_yaw, time_sec, 0.1f, 30.0f);
  // sweepUpdate(servo_pitch, time_sec, 0.1f, 10.0f, 90.0f);
  // delay(10);
}