#ifndef PAN_TILT_MANAGER_HPP
#define PAN_TILT_MANAGER_HPP

#ifdef FEETECH
#include "SCServo.h"
#else
#include <ESP32Servo.h>
#endif

namespace stackchan {

class PanTiltManager {
   private:
    uint8_t servo_pan_id_;   // yaw
    uint8_t servo_tilt_id_;  // pitch

    float pan_angle_;   // deg, current angle
    float tilt_angle_;  // deg, current angle
    // easing param
    float pan_target_angle_;   // deg
    float tilt_target_angle_;  // deg

    float pan_drive_start_angle_;
    float tilt_drive_start_angle_;
    float pan_drive_start_time_;   // timestamp in sec
    float tilt_drive_start_time_;  // timestamp in sec
    float pan_drive_complete_time_;
    float tilt_drive_complete_time_;

   public:
    PanTiltManager(uint8_t pan_id, uint8_t tilt_id);
    ~PanTiltManager();

#ifdef FEETECH
    SMS_STS st;
#else
    Servo servo_pan;
    Servo servo_tilt;
#endif

    void panDrive(float angle, float duration);
    void tiltDrive(float angle, float duration);

    void activate();
    void deactivate();

    void update();
};

PanTiltManager::PanTiltManager(uint8_t pan_id, uint8_t tilt_id)
    : servo_pan_id_(pan_id), servo_tilt_id_(tilt_id) {}

PanTiltManager::~PanTiltManager() {}

void PanTiltManager::activate() {
#ifdef FEETECH
    // todo
#else
    this->servo_pan.attach(this->servo_pan_id_);
    this->servo_tilt.attach(this->servo_tilt_id_);
#endif
}

void PanTiltManager::deactivate() {
#ifdef FEETECH
    // todo
#else
    this->servo_pan.detach();
    this->servo_tilt.detach();
#endif
}

void PanTiltManager::panDrive(float angle, float duration) {
    this->pan_drive_start_time_ = millis() / 1.0e3;
    this->pan_angle_ = this->pan_angle_;
    this->pan_drive_complete_time_ = this->pan_drive_start_time_ + duration;
}

}  // namespace stackchan

#endif