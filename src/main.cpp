
#include <ArduinoBLE.h>
#include <Avatar.h>
#include <M5Unified.h>
#include <TaskManager.h>

#include "BLEStackchanService.hpp"
#include "CustomFaces.hpp"
#include "PanTiltManager.hpp"

#ifdef FEETECH
#include "SCServo.h"
#else
#include <ESP32Servo.h>
#endif
m5avatar::Avatar avatar;
ble::StackchanService stackchan_srv;

const uint8_t servo_pan_id = 19;
const uint8_t servo_tilt_id = 27;
stackchan::PanTiltManager pan_tilt_manager(servo_pan_id, servo_tilt_id);

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
    m5avatar::Expression::Neutral, m5avatar::Expression::Happy,
    m5avatar::Expression::Angry,   m5avatar::Expression::Sad,
    m5avatar::Expression::Sleepy,  m5avatar::Expression::Doubt};
const uint8_t expressions_size = 6;
m5avatar::Expression current_expression = expressions[0];

m5avatar::ColorPalette* color_palettes[4];
const uint8_t color_palettes_size = 4U;
uint8_t color_palettes_idx = 0;

m5avatar::Face* faces[5];
const int faces_length = sizeof(faces) / sizeof(m5avatar::Face*);
int face_idx = 0;

void setup() {
#ifdef FEETECH
    Serial1.begin(1000000, SERIAL_8N1, 19, 27);
    st.pSerial = &Serial1;
#endif

#ifdef ARDUINO_M5STACK_CORES3
    auto cfg = M5.config();  // default config?
    cfg.output_power = false;
    M5.begin(cfg);
#endif

    faces[0] = avatar.getFace();
    faces[1] = new m5avatar::FbkFace();
    faces[2] = new m5avatar::GirlyFace();
    faces[3] = new m5avatar::GirlyFace2();
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
                           TFT_PINK);  // cheek
    // end of FBK Palette
    color_palettes[2]->set(COLOR_PRIMARY, TFT_DARKGREY);
    color_palettes[2]->set(COLOR_BACKGROUND, TFT_WHITE);
    color_palettes[3]->set(COLOR_PRIMARY, TFT_BLACK);
    color_palettes[3]->set(COLOR_BACKGROUND, TFT_PINK);

    avatar.init(8);  // start drawing w/ 8bit color mode

    if (!BLE.begin()) {
        // "starting BLE failed!"
        avatar.setSpeechText("BLE is unavailable");
    }
#ifdef FEETECH
    if (!Serial1.available()) {
        avatar.setSpeechText("Serial1 is unavailable");
    }
#endif

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

#ifdef FEETECH
    // init position
    st.RegWritePosEx(servo_pan_id, max_sweep / 2, speed, acc);
    st.RegWritePosEx(servo_tilt_id, max_sweep / 2, speed, acc);
    st.RegWriteAction();
    // delay(1884);  //[(P1-P0)/Speed]*1000+[Speed/(Acc*100)]*1000
#else
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    // servo_pan.setPeriodHertz(50);   // standard 50 hz servo
    // servo_tilt.setPeriodHertz(50);  // standard 50 hz servo

    stackchan_srv.pan_limit.min = 90 - 30;  // yaw
    stackchan_srv.pan_limit.max = 90 + 30;
    stackchan_srv.tilt_limit.min = 90 - 40;  // pitch
    stackchan_srv.tilt_limit.max = 90 + 10;
#endif

    // ## register tasks
    Tasks.setAutoErase(true);
    Tasks
        .add(
            "M5_update",
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
                 stackchan_srv.servoPoll(pan_tilt_manager);
             })
        ->startFps(10);
    Tasks
        .add("Facial_Update",
             [] {
                 stackchan_srv.facialExpressionPoll(avatar, expressions,
                                                    expressions_size);
                 stackchan_srv.facialColorPoll(avatar, color_palettes,
                                               color_palettes_size);
                 stackchan_srv.mouseOpenPoll(avatar);
             })
        ->startFps(30);

    Tasks.add("PanTilt_Update", [] { pan_tilt_manager.update(); })
        ->startFps(60);
}

void loop() {
    Tasks.update();  // automatically execute tasks

    delay(1);
}
