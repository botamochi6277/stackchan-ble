
#include <ArduinoBLE.h>
#include <Avatar.h>
#include <M5Unified.h>
#include <TaskManager.h>

#include <faces/FaceTemplates.hpp>

#include "Animation.hpp"
#include "BLEStackchanService.hpp"
#include "LeonaFace.hpp"
#include "STSServoDriver.h"

#if defined(ARDUINO_M5Stack_ATOM)
#define RXD 32
#define TXD 26
#elif defined(ARDUINO_M5Stack_Core_ESP32)
// port A
#define RXD 22  // SCL
#define TXD 21  // SDA
#elif defined(ARDUINO_M5STACK_Core2)
#define RXD 33  // SCL
#define TXD 32  // SDA
#elif defined(ARDUINO_XIAO_ESP32C3)
#define RXD 7
#define TXD 6
#endif

m5avatar::Avatar avatar;
char balloon_text[20];
ble::StackchanService stackchan_srv;

// STSServoDriver servo_driver;
#define ANIMATION_FPS 10
botamochi::AnimationController anim_controller(ANIMATION_FPS);
const uint8_t servo_pan_id = 1;
const uint8_t servo_tilt_id = 2;
unsigned short anim_clip_id = 0;
unsigned short tmp_pos[] = {IDLE_POSITION,
                            IDLE_POSITION + 50,  // slightly look up
                            IDLE_POSITION - 100, IDLE_POSITION};
unsigned short tmp_key[] = {0, 10, 20, 30};
botamochi::AnimationClip nob_clip(botamochi::JointName::kHeadTilt, tmp_pos,
                                  tmp_key, 4);
// NOTE https://qiita.com/dojyorin/items/4bf068aef2b248f1306e
botamochi::AnimationClip head_shake_clip(botamochi::JointName::kHeadPan,
                                         (unsigned short[]){IDLE_POSITION,
                                                            IDLE_POSITION - 100,
                                                            IDLE_POSITION + 100,
                                                            IDLE_POSITION},
                                         (unsigned short[]){0, 10, 20, 30}, 4);
// short max_sweep = 4095;
// short min_sweep = 0;
// unsigned short speed = 3400;
// unsigned char acc = 50;

// uint8_t pan_min = 60;   // deg
// uint8_t pan_max = 120;  // deg
// uint8_t tilt_min = 80;
// uint8_t tilt_max = 100;

float time_sec = 0.0f;
unsigned long milli_sec = 0U;

const m5avatar::Expression expressions[] = {
    m5avatar::Expression::kNeutral, m5avatar::Expression::kHappy,
    m5avatar::Expression::kAngry,   m5avatar::Expression::kSad,
    m5avatar::Expression::kSleepy,  m5avatar::Expression::kDoubt};
const uint8_t expressions_size = 6;
m5avatar::Expression current_expression = expressions[0];

m5avatar::ColorPalette* color_palettes[4];
const uint8_t color_palettes_size = 4U;
uint8_t color_palettes_idx = 0;

m5avatar::Face* faces[5];
const int faces_length = sizeof(faces) / sizeof(m5avatar::Face*);
int face_idx = 0;

void setup() {
  auto cfg = M5.config();  // default config?

#ifdef ARDUINO_M5STACK_CORES3
  cfg.output_power = false;
#endif
  M5.begin(cfg);
  Serial.begin(115200);
  Serial2.begin(1000000, SERIAL_8N1, RXD, TXD);
  delay(1000);  // waiting for connection

  M5.Lcd.setBrightness(100);
  M5.Lcd.clear();

  faces[0] = avatar.getFace();
  faces[1] = new m5avatar::LeonaFace();
  faces[2] = new m5avatar::GirlyFace();
  faces[3] = new m5avatar::OmegaFace();
  faces[4] = new m5avatar::PinkDemonFace();

  color_palettes[0] = new m5avatar::ColorPalette();
  color_palettes[1] = new m5avatar::ColorPalette();
  color_palettes[2] = new m5avatar::ColorPalette();
  color_palettes[3] = new m5avatar::ColorPalette();
  // FBK Palette
  color_palettes[1]->set(COLOR_PRIMARY,
                         M5.Lcd.color24to16(0x383838));  // eye
  color_palettes[1]->set(COLOR_BACKGROUND,
                         M5.Lcd.color24to16(0xfac2a8));  // skin
  color_palettes[1]->set(COLOR_SECONDARY,
                         M5.Lcd.color24to16(0xea9c60));  // cheek
  // end of FBK Palette
  color_palettes[2]->set(COLOR_PRIMARY, TFT_DARKGREY);
  color_palettes[2]->set(COLOR_BACKGROUND, TFT_WHITE);
  color_palettes[3]->set(COLOR_PRIMARY, TFT_BLACK);
  color_palettes[3]->set(COLOR_BACKGROUND, TFT_PINK);

  avatar.init(8);  // start drawing w/ 8bit color mode
  avatar.setColorPalette(*color_palettes[0]);

  if (!BLE.begin()) {
    // "starting BLE failed!"
    avatar.setSpeechText("BLE is unavailable");
  } else {
    M5_LOGD("BLE is available");
  }

  if (!Serial2) {
    avatar.setSpeechText("Serial2 is not connected");
  } else {
    M5_LOGD("Serial2 is connected");
  }

  // ## beginning Bluetooth setup
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

  // ## Servo setting
  auto is_connected = anim_controller.servo_driver.init(&Serial2);
  if (!is_connected) {
    avatar.setSpeechText("servo is not connected");
  } else {
    avatar.setSpeechText("servo is connected");
  }

  anim_controller.joint_servo_map.set(botamochi::JointName::kHeadPan,
                                      servo_pan_id);
  anim_controller.joint_servo_map.set(botamochi::JointName::kHeadTilt,
                                      servo_tilt_id);
  avatar.setSpeechText("Scaning servos...");
  for (size_t i = 1; i < 3; i++) {
    bool b = anim_controller.servo_driver.ping(i);
    anim_controller.servo_driver.getCurrentPosition(
        i);  // execute determineServoType
  }

  // initial move
  avatar.setSpeechText("moving to 400");
  anim_controller.servo_driver.setTargetPosition(servo_pan_id, 400);
  anim_controller.servo_driver.setTargetPosition(servo_tilt_id, 400);
  delay(3000);  // wait for servo to move
  avatar.setSpeechText("moving to 601");
  anim_controller.servo_driver.setTargetPosition(servo_pan_id, 601);
  anim_controller.servo_driver.setTargetPosition(servo_tilt_id, 601);
  delay(3000);  // wait for servo to move
  avatar.setSpeechText("moving to 511");
  anim_controller.servo_driver.setTargetPosition(servo_pan_id, IDLE_POSITION);
  anim_controller.servo_driver.setTargetPosition(servo_tilt_id, IDLE_POSITION);
  delay(3000);  // wait for servo to move

  // register animation clips
  anim_controller.setClip(0, nob_clip);
  anim_controller.setClip(1, head_shake_clip);

  // sprintf(balloon_text, "servo 01 pos: %d",
  //         anim_controller.servo_driver.getCurrentPosition(1));  // 14338???
  // avatar.setSpeechText(balloon_text);

  M5_LOGI("setting tasks...");
  // ## register tasks
  Tasks.setAutoErase(true);
  Tasks
      .add("M5_update",
           [] {
             M5.update();
             if (M5.BtnA.wasPressed()) {
               avatar.setFace(faces[face_idx]);
               face_idx = (face_idx + 1) % faces_length;
             }
             if (M5.BtnB.wasPressed()) {
               avatar.setColorPalette(*color_palettes[color_palettes_idx]);
               // have no effect on written flag
               // stackchan_srv.facial_color_chr.writeValue(color_palettes_idx);
               color_palettes_idx =
                   (color_palettes_idx + 1) % color_palettes_size;
             }
             if (M5.BtnC.wasPressed()) {
               anim_controller.play(anim_clip_id);
               anim_clip_id = (anim_clip_id + 1) % 2;
             }
           })
      ->startFps(100);
  Tasks
      .add("Clock",
           [] {
             milli_sec = millis();
             stackchan_srv.timer_chr.writeValue(milli_sec);
           })
      ->startFps(60);
  Tasks
      .add("BLE_polling",
           [] {
             BLE.poll();
             //  stackchan_srv.servoPoll(pan_tilt_manager);
           })
      ->startFps(10);
  Tasks
      .add("Facial_Update",
           [] {
             stackchan_srv.facePoll(avatar, faces, faces_length);
             stackchan_srv.facialExpressionPoll(avatar, expressions,
                                                expressions_size);
             stackchan_srv.facialColorPoll(avatar, color_palettes,
                                           color_palettes_size);
             stackchan_srv.mouseOpenPoll(avatar);
           })
      ->startFps(30);

  Tasks.add("Animation_Update", [] { anim_controller.update(); })
      ->startFps(ANIMATION_FPS);

  avatar.setSpeechText("Playing animation");
  anim_controller.play(0);
}

void loop() {
  Tasks.update();  // automatically execute tasks

  delay(1);
}
