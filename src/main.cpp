
#include <ArduinoBLE.h>
#include <M5Unified.h>
#include <STSServoDriver.h>
#include <TaskManager.h>

// stackchan display
#include <ColorPalette.h>
#include <Display.h>
#include <Expression.h>
#include <SpeechBalloon.h>

//

#include "Animation.hpp"
#include "AnimationClipExamples.hpp"
#include "BLEStackchanService.hpp"
#include "PaletteExamples.hpp"

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
#elif defined(ARDUINO_M5STACK_CORES3)
// #define RXD 1
// #define TXD 2
// TODO: Add Stackchan Body

#define RXD 7
#define TXD 6

#endif

using botamochi::AnimationName;

Display display;
char balloon_text[20];
ble::StackchanService stackchan_srv;

#define ANIMATION_FPS 10
botamochi::AnimationController anim_controller(ANIMATION_FPS);
const uint8_t servo_pan_id = 1;
const uint8_t servo_tilt_id = 2;
unsigned short anim_clip_id = 0;

float time_sec = 0.0f;
unsigned long milli_sec = 0U;
unsigned long m5_count = 0;
unsigned long main_count = 0;

const Expression expressions[] = {Expression::kNeutral, Expression::kSmile,
                                  Expression::kAngry,   Expression::kSad,
                                  Expression::kSleepy,  Expression::kDoubt};
const uint8_t expressions_size = 6;
Expression current_expression = expressions[0];
uint8_t expression_idx = 0;

ColorPalette* color_palettes[5];
const uint8_t color_palettes_size = 5U;
uint8_t color_palettes_idx = 0;

Face* faces[4];
const uint8_t faces_length = sizeof(faces) / sizeof(Face*);
uint8_t face_idx = 0;

void assignMiaPalette(ColorPalette* palette) {
  using stackchan::display::DrawingLocation;
  palette->set(DrawingLocation::kIris1,
               M5.Lcd.color24to16(0x4A99D9));  // iris 1, #4A99D9
  palette->set(DrawingLocation::kEyelash, M5.Lcd.color24to16(0x953346));
  palette->set(DrawingLocation::kEyelid,
               M5.Lcd.color24to16(0x63436F));  // #63436F
  palette->set(DrawingLocation::kIrisBackground, TFT_BLACK);
  palette->set(DrawingLocation::kIris2,
               M5.Lcd.color24to16(0xBFEDE8));  // iris2,#BFEDE8
  palette->set(DrawingLocation::kPupil,
               M5.Lcd.color24to16(0x2D493A));  // #2D493A
  palette->set(DrawingLocation::kEyeHighlight, TFT_WHITE);
  palette->set(DrawingLocation::kSkin,
               M5.Lcd.color24to16(0xfac2a8));  // skin
  palette->set(DrawingLocation::kEyeBrow,
               M5.Lcd.color24to16(0x63434F));  // #63434F
  palette->set(DrawingLocation::kMouthBackground, M5.Lcd.color24to16(0x63434F));
  palette->set(DrawingLocation::kInnerMouth,
               M5.Lcd.color24to16(0x8B0035));  // #8B0035
  palette->set(DrawingLocation::kCheek1,
               M5.Lcd.color24to16(0xd77398));  // cheek #d77398
}

void setup() {
  auto cfg = M5.config();  // default config?

  // please uncomment if you supply power from GROVE connector to the CORES3
  // #ifdef ARDUINO_M5STACK_CORES3
  //   cfg.output_power = false;
  // #endif

  M5.begin(cfg);
  M5.Log.setLogLevel(m5::log_target_serial, ESP_LOG_VERBOSE);
  // Serial.begin(115200);
  // Serial2.begin(1000000, SERIAL_8N1, RXD, TXD);  // for servo driver
  delay(1000);  // waiting for connection

  M5.Lcd.setBrightness(150);
  M5.Lcd.clear();

  M5.Display.drawString("Smart", 10, 10, 4);
  delay(200);
  M5.Display.drawString("Tech", 10, 50, 4);
  delay(200);
  M5.Display.drawString("Animating", 10, 90, 4);
  delay(200);
  M5.Display.drawString("Creative", 10, 130, 4);
  delay(200);
  M5.Display.drawString("Kit", 10, 170, 4);
  delay(200);
  M5.Display.drawString("CHANt", 10, 210, 4);

  delay(10000);

  // default
  faces[0] = display.getFace();
  color_palettes[0] = new ColorPalette();
  // leona
  faces[1] = new stackchan::display::ToonFace1();
  color_palettes[1] = new ColorPalette();
  assignLeonaPalette(color_palettes[1]);
  // ui
  faces[2] = new stackchan::display::ToonFace1();
  color_palettes[2] = new ColorPalette();
  assignUiPalette(color_palettes[2]);
  // fbk
  faces[3] = new stackchan::display::OmegaFace();
  color_palettes[3] = new ColorPalette();
  assignFbkPalette(color_palettes[3]);
  // pink demon
  // faces[4] = new PinkDemonFace();
  // color_palettes[4] = new ColorPalette();
  // assignPinkDemonPalette(color_palettes[4]);

  display.getCanvas().setColorDepth(8);  // start drawing w/ 8bit color mode
  display.setColorPalette(color_palettes[0]);
  display.setFace(faces[0]);
  // display.getFace()->autoScale();

  if (!BLE.begin()) {
    // "starting BLE failed!"
    display.getSpeechBalloon().setText("BLE is unavailable");
    display.update();
    M5_LOGW("BLE is unavailable");
  } else {
    M5_LOGD("BLE is available");
  }

  if (!Serial2) {
    display.getSpeechBalloon().setText("Serial2 is not connected");
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

  // // ## Servo setting
  auto is_connected = anim_controller.servo_driver.init(&Serial2);
  if (!is_connected) {
    display.getSpeechBalloon().setText("servo is not connected");
  } else {
    display.getSpeechBalloon().setText("servo is connected");
  }
  display.update();

  anim_controller.joint_servo_map.set(botamochi::JointName::kHeadPan,
                                      servo_pan_id);
  anim_controller.joint_servo_map.set(botamochi::JointName::kHeadTilt,
                                      servo_tilt_id);
  display.getSpeechBalloon().setText("Scaning servos...");
  for (size_t i = 1; i < 3; i++) {
    bool b = anim_controller.servo_driver.ping(i);
    anim_controller.servo_driver.getCurrentPosition(
        i);  // execute determineServoType
  }

  // initial move
  display.getSpeechBalloon().setText("moving to 400");
  anim_controller.servo_driver.setTargetPosition(servo_pan_id, 400);
  anim_controller.servo_driver.setTargetPosition(servo_tilt_id, 400);
  display.update();
  delay(3000);  // wait for servo to move
  display.getSpeechBalloon().setText("moving to 601");
  anim_controller.servo_driver.setTargetPosition(servo_pan_id, 601);
  anim_controller.servo_driver.setTargetPosition(servo_tilt_id, 601);
  display.update();
  delay(3000);  // wait for servo to move
  display.getSpeechBalloon().setText("moving to 511");
  anim_controller.servo_driver.setTargetPosition(servo_pan_id, IDLE_POSITION);
  anim_controller.servo_driver.setTargetPosition(servo_tilt_id, IDLE_POSITION);
  display.update();
  delay(3000);  // wait for servo to move

  // register animation clips
  anim_controller.setClip((unsigned short)AnimationName::kLookFront,
                          botamochi::look_front_clip);
  anim_controller.setClip((unsigned short)AnimationName::kLookUp,
                          botamochi::look_up_clip);
  anim_controller.setClip((unsigned short)AnimationName::kLookDown,
                          botamochi::look_down_clip);
  anim_controller.setClip((unsigned short)AnimationName::kLookLeft,
                          botamochi::look_left_clip);
  anim_controller.setClip((unsigned short)AnimationName::kLookRight,
                          botamochi::look_right_clip);
  anim_controller.setClip((unsigned short)AnimationName::kNod,
                          botamochi::nod_clip);
  anim_controller.setClip((unsigned short)AnimationName::kShake,
                          botamochi::head_shake_clip);

  // sprintf(balloon_text, "servo 01 pos: %d",
  //         anim_controller.servo_driver.getCurrentPosition(1));  // 14338???
  // display.getSpeechBalloon().setText(balloon_text);

  M5_LOGI("setting tasks...");
  // ## register tasks
  Tasks.setAutoErase(true);
  // Tasks
  // .add("M5_update",
  //      [] {
  //        //  M5.update();
  //        //  if (M5.BtnA.wasPressed()) {
  //        //    display.setFace(faces[face_idx]);
  //        //    face_idx = (face_idx + 1) % faces_length;
  //        //    display.setColorPalette(color_palettes[color_palettes_idx]);
  //        //    color_palettes_idx =
  //        //        (color_palettes_idx + 1) % color_palettes_size;
  //        //  }
  //        //  if (M5.BtnB.wasPressed()) {
  //        //    display.getExpressionWeight().set(expressions[expression_idx],
  //        //                                      255);
  //        //    expression_idx = (expression_idx + 1) % expressions_size;
  //        //  }
  //        //  if (M5.BtnC.wasPressed()) {
  //        //    anim_controller.play(anim_clip_id);
  //        //    anim_clip_id = (anim_clip_id + 1) % 7;
  //        //  }

  //        //  if (m5_count % (100 * 60) == 0) {
  //        //    auto i = random(7);
  //        //    anim_controller.play(i);
  //        //  }

  //        // random motion
  //        m5_count += 1;
  //      })
  // ->startFps(100);
  // Tasks
  //     .add("Clock",
  //          [] {
  //            milli_sec = millis();
  //            stackchan_srv.timer_chr.writeValue(milli_sec);
  //          })
  //     ->startFps(60);
  Tasks
      .add("BLE_polling",
           [] {
             BLE.poll();
             stackchan_srv.animationPoll(anim_controller);
           })
      ->startFps(10);
  Tasks
      .add("Facial_Update",
           [] {
             M5.update();
             //  stackchan_srv.facePoll(display, faces, faces_length);
             //  stackchan_srv.facialExpressionPoll(display, expressions,
             //                                     expressions_size);
             //  stackchan_srv.facialColorPoll(display, color_palettes,
             //                                color_palettes_size);
             //  stackchan_srv.mouseOpenPoll(display);
             display.update();
             //  M5_LOGI("Face updated");
           })
      ->startFps(30);

  Tasks
      .add("Animation_Update",
           [] {
             anim_controller.update();
             stackchan_srv.servoPoll(anim_controller);
           })
      ->startFps(ANIMATION_FPS);

  display.getSpeechBalloon().setText("");  // hide speech balloon
  anim_controller.play((unsigned short)AnimationName::kNod);
}

void loop() {
  Tasks.update();  // automatically execute tasks

  delay(1);
}
