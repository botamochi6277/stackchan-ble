#ifndef PAN_TILT_MANAGER_HPP
#define PAN_TILT_MANAGER_HPP

#include <TaskManager.h>

#include <cmath>

#ifdef FEETECH
#include "SCServo.h"
#else
#include <ESP32Servo.h>
#endif

namespace stackchan {
template <typename T>
T remap(T x, T in_min, T in_max, T out_min, T out_max, bool clip = false) {
    if (clip) {
        if (x < in_min) {
            return out_min;
        }
        if (in_max < x) {
            return out_max;
        }
    }

    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float quadInOut(float x, float duration = 1.0f) {
    float xx = x / (duration + 1e-9f);
    if (x < 0.0f) {
        return 0.0f;
    } else if (x < 0.5f * duration) {
        return 2.0f * xx * xx;
    } else if (xx < duration) {
        return (1.0f - std::pow(-2.0f * xx + 2.0f, 2.0f) / 2.0f);
    } else {
        return 1.0f;
    }
}

class PanTiltManager {
   private:
    uint8_t servo_pan_id_;   // yaw
    uint8_t servo_tilt_id_;  // pitch
    float control_freq_;     // Hz

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
    PanTiltManager(uint8_t pan_id, uint8_t tilt_id, float ctrl_frq);
    ~PanTiltManager();

#ifdef FEETECH
    SMS_STS st;
#else
    Servo servo_pan;
    Servo servo_tilt;
#endif

    void panDrive(float angle, float duration);
    void tiltDrive(float angle, float duration);
    void setPanAngle(float angle) { this->pan_angle_ = angle; };
    void setTiltAngle(float angle) { this->tilt_angle_ = angle; };

    void activate();
    void deactivate();

    void update();
};

PanTiltManager::PanTiltManager(uint8_t pan_id, uint8_t tilt_id, float ctrl_frq)
    : servo_pan_id_(pan_id), servo_tilt_id_(tilt_id), control_freq_(ctrl_frq) {
    this->pan_angle_ = 90.0f;
    this->tilt_angle_ = 90.0f;
}

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
    this->pan_drive_complete_time_ = this->pan_drive_start_time_ + duration;
    this->pan_drive_start_angle_ = this->pan_angle_;
    this->pan_target_angle_ = angle;
}

void PanTiltManager::update() {
    float t = millis() / 1.0e3;
    // pan
    if (t < this->pan_drive_complete_time_) {
        float duration =
            this->pan_drive_complete_time_ - this->pan_drive_start_time_;
        float elapse_time = t - this->pan_drive_start_time_;
        float m = quadInOut(elapse_time, duration);
        float current_angle = remap(m, 0.0f, 1.0f, this->pan_drive_start_angle_,
                                    this->pan_target_angle_);
        this->servo_pan.write(static_cast<int>(current_angle));
        this->pan_angle_ = current_angle;
    }
    // tilt
    if (t < this->tilt_drive_complete_time_) {
        float duration =
            this->tilt_drive_complete_time_ - this->tilt_drive_start_time_;
        float elapse_time = t - this->pan_drive_start_angle_;
        float m = quadInOut(elapse_time, duration);
        float current_angle =
            remap(m, 0.0f, 1.0f, this->tilt_drive_start_angle_,
                  this->tilt_target_angle_);
        this->servo_tilt.write(static_cast<int>(current_angle));
        this->tilt_angle_ = current_angle;
    }
}

}  // namespace stackchan

#endif