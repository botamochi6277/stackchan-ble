// BLE service for stackchan

#ifndef BLE_STACKCHAN_SERVICE_HPP
#define BLE_STACKCHAN_SERVICE_HPP

#include <ArduinoBLE.h>
#include <Avatar.h>
#include <STSServoDriver.h>

#include "BLEFormat.hpp"
#include "BLEUnit.hpp"

namespace color {
u_int16_t to16bitscolor(u_int32_t color) {
  // https://docs.arduino.cc/library-examples/tft-library/TFTColorPicker
  u_int8_t r = (0x00FF0000 & color) >> 16;
  u_int8_t g = (0x0000FF00 & color) >> 8;
  u_int8_t b = (0x000000FF & color);

  // 5bits red and blue
  r = 0b11111 * (static_cast<float>(r) / 255.0f);
  b = 0b11111 * (static_cast<float>(b) / 255.0f);
  // 6bits green
  g = 0b111111 * (static_cast<float>(g) / 255.0f);

  return r << 11 | g << 5 | b;
}
}  // namespace color

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
  const uint8_t deg_format_[7] = {BLE_GATT_CPF_FORMAT_UINT8,
                                  0,
                                  (uint8_t)BLE_GATT_CPF_UNIT_DEGREE,
                                  (uint8_t)(BLE_GATT_CPF_UNIT_DEGREE >> 8),
                                  0x01,
                                  0x00,
                                  0x00};

  const uint8_t cmd_format_[7] = {BLE_GATT_CPF_FORMAT_UINT8,
                                  0,
                                  (uint8_t)BLE_GATT_CPF_UNIT_UNITLESS,
                                  (uint8_t)(BLE_GATT_CPF_UNIT_UNITLESS >> 8),
                                  0x01,
                                  0x00,
                                  0x00};
  const uint8_t rgb_format_[7] = {BLE_GATT_CPF_FORMAT_UINT32,
                                  0,
                                  (uint8_t)BLE_GATT_CPF_UNIT_RGB,
                                  (uint8_t)(BLE_GATT_CPF_UNIT_RGB >> 8),
                                  0x01,
                                  0x00,
                                  0x00};

 public:
  Limitation pan_limit;
  Limitation tilt_limit;

  //  characteristics
  BLEUnsignedLongCharacteristic timer_chr;

  // facial style
  BLEUnsignedCharCharacteristic face_chr;

  // facial motion
  BLEUnsignedCharCharacteristic facial_expression_chr;
  BLEUnsignedCharCharacteristic facial_color_chr;
  BLEUnsignedCharCharacteristic mouse_open_ratio_chr;

  BLEUnsignedIntCharacteristic primary_color_chr;
  // (todo) secondary color
  BLEUnsignedIntCharacteristic background_color_chr;
  // (todo) balloon foreground
  // (todo) balloon background

  // servo
  BLEBooleanCharacteristic is_servo_activated_chr;
  BLEUnsignedCharCharacteristic servo_pan_angle_chr;
  BLEUnsignedCharCharacteristic servo_tilt_angle_chr;

  StackchanService(/* args */);
  // ~StackchanService();
  void setInitialValues();

  // void servoPoll(stackchan::PanTiltManager &manager);

  void facePoll(m5avatar::Avatar &avatar, m5avatar::Face *faces[],
                uint8_t face_size);

  void facialExpressionPoll(m5avatar::Avatar &avatar,
                            const m5avatar::Expression expressions[],
                            uint8_t expression_size);

  void facialColorPoll(m5avatar::Avatar &avatar,
                       m5avatar::ColorPalette *palettes[],
                       uint8_t palette_size);

  void mouseOpenPoll(m5avatar::Avatar &avatar);
};

StackchanService::StackchanService()
    : BLEService("671e0000-8cef-46b7-8af3-2eddeb12803e"),
      timer_chr("671e0001-8cef-46b7-8af3-2eddeb12803e", BLERead | BLENotify),
      face_chr("671e0100-8cef-46b7-8af3-2eddeb12803e", BLERead | BLEWrite),
      facial_expression_chr("671e1000-8cef-46b7-8af3-2eddeb12803e",
                            BLERead | BLEWrite),
      facial_color_chr("671e1002-8cef-46b7-8af3-2eddeb12803e",
                       BLERead | BLEWrite),
      mouse_open_ratio_chr("671e1003-8cef-46b7-8af3-2eddeb12803e",
                           BLERead | BLEWrite),
      primary_color_chr("671e1004-8cef-46b7-8af3-2eddeb12803e",
                        BLERead | BLEWrite),
      background_color_chr("671e1005-8cef-46b7-8af3-2eddeb12803e",
                           BLERead | BLEWrite),
      is_servo_activated_chr("671e2000-8cef-46b7-8af3-2eddeb12803e",
                             BLERead | BLEWrite),
      servo_pan_angle_chr("671e2001-8cef-46b7-8af3-2eddeb12803e",
                          BLERead | BLEWrite),
      servo_tilt_angle_chr("671e2002-8cef-46b7-8af3-2eddeb12803e",
                           BLERead | BLEWrite) {
  // add characteristics to service
  this->addCharacteristic(this->timer_chr);
  this->addCharacteristic(this->face_chr);
  this->addCharacteristic(this->facial_expression_chr);
  this->addCharacteristic(this->facial_color_chr);
  this->addCharacteristic(this->mouse_open_ratio_chr);
  this->addCharacteristic(this->primary_color_chr);
  this->addCharacteristic(this->background_color_chr);
  this->addCharacteristic(this->is_servo_activated_chr);
  this->addCharacteristic(this->servo_pan_angle_chr);
  this->addCharacteristic(this->servo_tilt_angle_chr);

  // User Description
  BLEDescriptor timer_descriptor("2901", "timer_ms");
  this->timer_chr.addDescriptor(timer_descriptor);

  BLEDescriptor face_descriptor("2901", "face");
  this->face_chr.addDescriptor(face_descriptor);

  BLEDescriptor facial_expression_descriptor("2901", "facial expression");
  this->facial_expression_chr.addDescriptor(facial_expression_descriptor);
  BLEDescriptor facial_color_descriptor("2901", "facial color");
  this->facial_color_chr.addDescriptor(facial_color_descriptor);
  BLEDescriptor mouse_or_descriptor("2901", "mouse open ratio");
  this->mouse_open_ratio_chr.addDescriptor(mouse_or_descriptor);

  BLEDescriptor primary_color_descriptor("2901", "primary color");
  this->primary_color_chr.addDescriptor(primary_color_descriptor);
  BLEDescriptor bg_color_descriptor("2901", "background color");
  this->background_color_chr.addDescriptor(bg_color_descriptor);

  BLEDescriptor pwr_descriptor("2901", "is_servo_activated");
  this->is_servo_activated_chr.addDescriptor(pwr_descriptor);
  BLEDescriptor pan_descriptor("2901", "pan angle [deg]");
  this->servo_pan_angle_chr.addDescriptor(pan_descriptor);
  BLEDescriptor tilt_descriptor("2901", "tilt angle [deg]");
  this->servo_tilt_angle_chr.addDescriptor(tilt_descriptor);

  // Format Description
  BLEDescriptor millisec_descriptor("2904", this->msec_format_, 7);
  this->timer_chr.addDescriptor(millisec_descriptor);

  BLEDescriptor face_fmt_descriptor("2904", this->cmd_format_, 7);
  this->face_chr.addDescriptor(face_fmt_descriptor);

  BLEDescriptor facial_exp_descriptor("2904", this->cmd_format_, 7);
  this->facial_expression_chr.addDescriptor(facial_exp_descriptor);

  BLEDescriptor facial_color_fmt_descriptor("2904", this->cmd_format_, 7);
  this->facial_color_chr.addDescriptor(facial_color_fmt_descriptor);

  BLEDescriptor primary_color_fmt_descriptor("2904", this->rgb_format_, 7);
  this->primary_color_chr.addDescriptor(primary_color_fmt_descriptor);
  BLEDescriptor bg_color_fmt_descriptor("2904", this->rgb_format_, 7);
  this->background_color_chr.addDescriptor(bg_color_fmt_descriptor);

  BLEDescriptor mouse_open_descriptor("2904", this->cmd_format_, 7);
  this->mouse_open_ratio_chr.addDescriptor(mouse_open_descriptor);

  BLEDescriptor servo_activate_descriptor("2904", this->cmd_format_, 7);
  this->is_servo_activated_chr.addDescriptor(servo_activate_descriptor);
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

  this->primary_color_chr.writeValueLE(0xffffff);
  this->background_color_chr.writeValueLE(0x000000);
};

void StackchanService::facePoll(m5avatar::Avatar &avatar,
                                m5avatar::Face *faces[], uint8_t face_size) {
  if (this->face_chr.written()) {
    auto idx = this->face_chr.value();
    if (face_size <= idx) {
      return;
    }

    avatar.setFace(faces[idx]);
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

  if (this->primary_color_chr.written() ||
      this->background_color_chr.written()) {
    auto idx = this->facial_color_chr.value();
    // convert 24 bit color to 16bit color
    palettes[idx]->set(COLOR_PRIMARY,
                       color::to16bitscolor(this->primary_color_chr.valueLE()));
    palettes[idx]->set(
        COLOR_BACKGROUND,
        color::to16bitscolor(this->background_color_chr.valueLE()));
    avatar.setColorPalette(*palettes[idx]);
  }
}

void StackchanService::mouseOpenPoll(m5avatar::Avatar &avatar) {
  if (this->mouse_open_ratio_chr.written()) {
    avatar.setMouthOpenRatio(
        static_cast<float>(this->mouse_open_ratio_chr.value()) / 255U);
  }
}

}  // namespace ble
#endif
