// BLE service for stackchan

#ifndef BLE_STACKCHAN_SERVICE_HPP
#define BLE_STACKCHAN_SERVICE_HPP

#include <ArduinoBLE.h>
#include <Avatar.h>
#include <ESP32Servo.h>

#include "BLEFormat.hpp"
#include "BLEUnit.hpp"

namespace ble {

struct Limitation {
  uint8_t min;
  uint8_t max;
};

class StackchanService : public BLEService {
 private:
  const uint8_t msec_format_[7] = {
      BLE_GATT_CPF_FORMAT_UINT64,
      0b11111101,                             // exp, milli, -3
      (uint8_t)BLE_GATT_CPF_UNIT_SEC,         // 0x13
      (uint8_t)(BLE_GATT_CPF_UNIT_SEC >> 8),  // 0x27
      0x01,
      0x00,
      0x00};
  const uint8_t deg_format_[7] = {BLE_GATT_CPF_FORMAT_UINT64,
                                  0,
                                  (uint8_t)BLE_GATT_CPF_UNIT_DEGREE,
                                  (uint8_t)(BLE_GATT_CPF_UNIT_DEGREE >> 8),
                                  0x01,
                                  0x00,
                                  0x00};

 public:
  Limitation pan_limit;
  Limitation tilt_limit;

  //  characteristics
  BLEUnsignedLongCharacteristic timer_chr;

  // facial
  BLEUnsignedCharCharacteristic facial_expression_chr;
  BLEUnsignedCharCharacteristic facial_color_chr;

  // servo
  BLEBooleanCharacteristic is_servo_activated_chr;
  BLEUnsignedCharCharacteristic servo_pan_angle_chr;
  BLEUnsignedCharCharacteristic servo_tilt_angle_chr;

  StackchanService(/* args */);
  // ~StackchanService();
  void setInitialValues();
  void servoPoll(Servo &servo_pan, Servo &servo_tilt, uint8_t pan_pin = 32,
                 uint8_t tilt_pin = 33);

  void facialExpressionPoll(m5avatar::Avatar &avatar,
                            const m5avatar::Expression expressions[],
                            uint8_t expression_size);

  void facialColorPoll(m5avatar::Avatar &avatar,
                       m5avatar::ColorPalette *palettes[],
                       uint8_t palette_size);
};

StackchanService::StackchanService()
    : BLEService("671e0000-8cef-46b7-8af3-2eddeb12803e"),
      timer_chr("671e0001-8cef-46b7-8af3-2eddeb12803e", BLERead | BLENotify),
      facial_expression_chr("671e1000-8cef-46b7-8af3-2eddeb12803e",
                            BLERead | BLEWrite),
      facial_color_chr("671e1002-8cef-46b7-8af3-2eddeb12803e",
                       BLERead | BLEWrite),
      is_servo_activated_chr("671e2000-8cef-46b7-8af3-2eddeb12803e",
                             BLERead | BLEWrite),
      servo_pan_angle_chr("671e2000-8cef-46b7-8af3-2eddeb12803e",
                          BLERead | BLEWrite),
      servo_tilt_angle_chr("671e2000-8cef-46b7-8af3-2eddeb12803e",
                           BLERead | BLEWrite) {
  // add characteristics to service
  this->addCharacteristic(this->timer_chr);
  this->addCharacteristic(this->facial_expression_chr);
  this->addCharacteristic(this->facial_color_chr);
  this->addCharacteristic(this->is_servo_activated_chr);
  this->addCharacteristic(this->servo_pan_angle_chr);
  this->addCharacteristic(this->servo_tilt_angle_chr);

  // User Description
  BLEDescriptor timer_descriptor("2901", "timer_ms");
  this->timer_chr.addDescriptor(timer_descriptor);

  BLEDescriptor facial_descriptor("2901", "facial expression");
  this->facial_expression_chr.addDescriptor(facial_descriptor);
  BLEDescriptor facial_color_descriptor("2901", "facial color");
  this->facial_color_chr.addDescriptor(facial_color_descriptor);

  BLEDescriptor pwr_descriptor("2901", "is_servo_activated");
  this->is_servo_activated_chr.addDescriptor(pwr_descriptor);
  BLEDescriptor pan_descriptor("2901", "pan angle [deg]");
  this->servo_pan_angle_chr.addDescriptor(pan_descriptor);
  BLEDescriptor tilt_descriptor("2901", "tilt angle [deg]");
  this->servo_tilt_angle_chr.addDescriptor(tilt_descriptor);

  // Format Description
  BLEDescriptor millisec_descriptor("2904", this->msec_format_, 7);
  this->timer_chr.addDescriptor(millisec_descriptor);

  BLEDescriptor angle_pan_descriptor01("2904", this->deg_format_, 7);
  this->servo_pan_angle_chr.addDescriptor(angle_pan_descriptor01);
  BLEDescriptor angle_tilt_descriptor01("2904", this->deg_format_, 7);
  this->servo_tilt_angle_chr.addDescriptor(angle_tilt_descriptor01);
};

void StackchanService::setInitialValues() {
  this->timer_chr.writeValue(0U);
  this->is_servo_activated_chr.writeValue(false);
  this->servo_pan_angle_chr.writeValue(90U);
  this->servo_tilt_angle_chr.writeValue(90U);
};

void StackchanService::servoPoll(Servo &servo_pan, Servo &servo_tilt,
                                 uint8_t pan_pin, uint8_t tilt_pin) {
  if (this->is_servo_activated_chr.written()) {
    if (this->is_servo_activated_chr.value()) {
      // activate servo
      servo_pan.attach(pan_pin);
      servo_tilt.attach(tilt_pin);
    } else {
      servo_pan.detach();
      servo_pan.detach();
    }
  }

  if (this->is_servo_activated_chr.value()) {
    if (this->servo_pan_angle_chr.written()) {
      auto angle = this->servo_pan_angle_chr.value();
      if (angle < this->pan_limit.min) {
        angle = this->pan_limit.min;
      }
      if (this->pan_limit.max < angle) {
        angle = this->pan_limit.max;
      }
      servo_pan.write(angle);
    }
    if (this->servo_tilt_angle_chr.written()) {
      auto angle = this->servo_tilt_angle_chr.value();
      if (angle < this->tilt_limit.min) {
        angle = this->tilt_limit.min;
      }
      if (this->tilt_limit.max < angle) {
        angle = this->tilt_limit.max;
      }
      servo_tilt.write(angle);
    }
  }
}

void StackchanService::facialExpressionPoll(
    m5avatar::Avatar &avatar, const m5avatar::Expression expressions[],
    uint8_t expression_size) {
  if (this->facial_expression_chr.written()) {
    auto idx = this->facial_expression_chr.value();
    if (expression_size <= idx) {
      return;
    }

    avatar.setExpression(expressions[idx]);
  }
}

void StackchanService::facialColorPoll(m5avatar::Avatar &avatar,
                                       m5avatar::ColorPalette *palettes[],
                                       uint8_t palette_size) {
  if (this->facial_color_chr.written()) {
    auto idx = this->facial_color_chr.value();
    if (palette_size <= idx) {
      return;  // out of index
    }
    avatar.setColorPalette(*palettes[idx]);
  }
}

}  // namespace ble
#endif
