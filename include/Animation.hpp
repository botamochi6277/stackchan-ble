
#ifndef BOTAMOCHI_ANIMATION_HPP
#define BOTAMOCHI_ANIMATION_HPP

#define ANIM_BUFF_LENGTH 8
#define IDLE_POSITION 511
#define MAX_ANIM_CLIP_NUM 8
#define NUM_JOINTS 2

#include <STSServoDriver.h>

#include <map>

namespace botamochi {

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

enum class JointName : unsigned char {
  kHeadPan = 0,
  kHeadTilt,
};

enum class AnimationName : unsigned char {
  kLookFront = 0,
  kLookUp,
  kLookDown,
  kLookLeft,
  kLookRight,
  kNod,
  kShake,
};

class AnimationClip {
 private:
  JointName joint_names_[NUM_JOINTS];
  unsigned short positions_[NUM_JOINTS][ANIM_BUFF_LENGTH];
  unsigned short speeds_[NUM_JOINTS][ANIM_BUFF_LENGTH];
  unsigned short keyframes_[ANIM_BUFF_LENGTH];
  unsigned short length_;    // num values of the clip
  unsigned short duration_;  // in frames
  unsigned int last_play_frame_id_;
  unsigned short num_played_;

 public:
  AnimationClip();
  AnimationClip(JointName joint_names[],
                unsigned short positions[][ANIM_BUFF_LENGTH],
                unsigned short keyframes[], unsigned short length);
  ~AnimationClip();
  inline JointName getJointName(unsigned short idx) {
    return joint_names_[idx];
  }

  bool isPlaying(unsigned short current_frame_id);
  void play(unsigned short current_frame_id, bool is_ease_in = false);

  void emulate_position_and_speed(unsigned short current_frame_id,
                                  JointName joint_name,
                                  unsigned short& position,
                                  unsigned short& speed,
                                  unsigned short ease_in_position = -1);
};

AnimationClip::AnimationClip() {
  // init values
  joint_names_[0] = JointName::kHeadPan;
  joint_names_[1] = JointName::kHeadTilt;
  positions_[0][0] = IDLE_POSITION;
  positions_[0][1] = IDLE_POSITION;
  positions_[1][0] = IDLE_POSITION;
  positions_[1][1] = IDLE_POSITION;
  speeds_[0][0] = IDLE_POSITION;
  speeds_[0][1] = IDLE_POSITION;
  speeds_[1][0] = IDLE_POSITION;
  speeds_[1][1] = IDLE_POSITION;
  keyframes_[0] = 0;
  keyframes_[1] = 10;
  length_ = 2;
  last_play_frame_id_ = 0;
  num_played_ = 0;
  duration_ = 10;
}
AnimationClip::AnimationClip(JointName joint_names[],
                             unsigned short positions[][ANIM_BUFF_LENGTH],
                             unsigned short keyframes[],
                             unsigned short length) {
  for (size_t joint_idx = 0; joint_idx < NUM_JOINTS; joint_idx++) {
    joint_names_[joint_idx] = joint_names[joint_idx];
    for (unsigned short i = 0; i < length; i++) {
      if (length >= ANIM_BUFF_LENGTH) {
        break;
      }

      this->positions_[joint_idx][i] = positions[joint_idx][i];
      this->keyframes_[i] = keyframes[i];
      if (i == 0) {
        this->speeds_[joint_idx][i] =
            positions[joint_idx][i + 1] > positions[joint_idx][0]
                ? (positions[joint_idx][i + 1] - positions[joint_idx][0])
                : (positions[joint_idx][0] - positions[joint_idx][i + 1]);
      } else if (i == length - 1) {
        this->speeds_[joint_idx][i] =
            positions[joint_idx][i] > positions[joint_idx][i - 1]
                ? (positions[joint_idx][i] - positions[joint_idx][i - 1])
                : (positions[joint_idx][i - 1] - positions[joint_idx][i]);
      } else {
        this->speeds_[joint_idx][i] =
            positions[joint_idx][i + 1] > positions[joint_idx][i - 1]
                ? (positions[joint_idx][i + 1] - positions[joint_idx][i - 1]) /
                      2
                : (positions[joint_idx][i - 1] - positions[joint_idx][i + 1]) /
                      2;
      }
    }
  }

  length_ = length;
  last_play_frame_id_ = 0;  // will start playing from 1
  num_played_ = 0;
  duration_ = keyframes[length_ - 1] - keyframes[0];
}

bool AnimationClip::isPlaying(unsigned short current_frame_id) {
  if (num_played_ == 0) {
    return false;
  }
  // last_play_frame_id_ <= current_frame_id < last_play_frame_id_ + duration_

  return ((last_play_frame_id_ <= current_frame_id) &&
          (current_frame_id < last_play_frame_id_ + duration_));
}

void AnimationClip::play(unsigned short current_frame_id, bool is_ease_in) {
  num_played_ += 1;
  last_play_frame_id_ = current_frame_id;
};

void AnimationClip::emulate_position_and_speed(
    unsigned short current_frame_id, JointName joint_name,
    unsigned short& position, unsigned short& speed,
    unsigned short ease_in_position) {
  bool is_ease_in = (ease_in_position != -1);
  // before initial keyframe
  if (this->last_play_frame_id_ > current_frame_id) {
    if (is_ease_in) {
      position = ease_in_position;
    } else {
      position = positions_[(uint8_t)joint_name][0];
      speed = speeds_[(uint8_t)joint_name][0];
    }

    return;
  }

  unsigned short t = current_frame_id - this->last_play_frame_id_;
  // after last keyframe
  if (t > this->keyframes_[length_ - 1]) {
    position = positions_[(uint8_t)joint_name][this->length_ - 1];
    speed = speeds_[(uint8_t)joint_name][this->length_ - 1];
    return;
  }

  for (unsigned short i = 1; i < this->length_; i++) {
    // find i meeting (keyframes[i-1] < t < keyframes[i])
    if (t < keyframes_[i - 1] || keyframes_[i] < t) {
      continue;
    }

    if (i == 1 && is_ease_in) {
      // position[0] is replaced by last_emulated_position_
      position = remap(t, keyframes_[i - 1], keyframes_[i], ease_in_position,
                       positions_[(uint8_t)joint_name][i]);
      unsigned short tmp_speed =
          positions_[(uint8_t)joint_name][i] > ease_in_position
              ? (positions_[(uint8_t)joint_name][i] - ease_in_position)
              : (ease_in_position - positions_[(uint8_t)joint_name][i]);
      speed = remap(t, keyframes_[i - 1], keyframes_[i], tmp_speed,
                    speeds_[(uint8_t)joint_name][i]);
      return;
    }

    // interpolate
    position = remap(t, keyframes_[i - 1], keyframes_[i],
                     positions_[(uint8_t)joint_name][i - 1],
                     positions_[(uint8_t)joint_name][i]);
    speed = remap(t, keyframes_[i - 1], keyframes_[i],
                  speeds_[(uint8_t)joint_name][i - 1],
                  speeds_[(uint8_t)joint_name][i]);
    return;
  }
}

AnimationClip::~AnimationClip() {}

// servo_id--->joint_name mapping
class JointServoMap {
 private:
  std::map<JointName, uint16_t> map_;

 public:
  JointServoMap(/* args */);
  ~JointServoMap();
  void set(JointName joint_name, uint16_t servo_id);
  uint16_t get(JointName joint_name);
  bool contains(JointName joint_name);
};

JointServoMap::JointServoMap(/* args */) {}

JointServoMap::~JointServoMap() {}

void JointServoMap::set(JointName joint_name, uint16_t servo_id) {
  map_.insert(std::make_pair(joint_name, servo_id));
}

uint16_t JointServoMap::get(JointName joint_name) {
  auto itr = map_.find(joint_name);
  if (itr != map_.end()) {
    return itr->second;
  } else {
    return 0;
  }
}

bool JointServoMap::contains(JointName joint_name) {
  auto itr = map_.find(joint_name);
  return itr != map_.end();
}

class AnimationController {
 private:
  unsigned int step_;
  unsigned short fps_;
  AnimationClip clips_[MAX_ANIM_CLIP_NUM];
  // current_position
  unsigned short last_written_positions_[NUM_JOINTS];
  // prev_position

 public:
  STSServoDriver servo_driver;
  JointServoMap joint_servo_map;

  AnimationController(unsigned short fps);
  ~AnimationController();
  void update();
  void setClip(unsigned short id, AnimationClip& clip);
  void play(unsigned short clip_id);
};

AnimationController::AnimationController(unsigned short fps)
    : fps_(fps), last_written_positions_({IDLE_POSITION, IDLE_POSITION}) {}

AnimationController::~AnimationController() {}

void AnimationController::setClip(unsigned short id, AnimationClip& clip) {
  this->clips_[id] = clip;
}

void AnimationController::play(unsigned short clip_id) {
  // play from current frame, set playing frame_id
  this->clips_[clip_id].play(step_, true);
}

void AnimationController::update() {
  step_ += 1;  // start from 1
  unsigned short pos;
  unsigned short speed;
  for (unsigned short clip_id = 0; clip_id < MAX_ANIM_CLIP_NUM; clip_id++) {
    if (!this->clips_[clip_id].isPlaying(step_)) {
      continue;
    }

    for (size_t i = 0; i < NUM_JOINTS; i++) {
      auto joint_name = this->clips_[clip_id].getJointName(i);
      // TODO: try to get position from servo IC
      this->clips_[clip_id].emulate_position_and_speed(
          step_, joint_name, pos, speed, last_written_positions_[i]);

      // speed [pulse/frame] -> [pulse/sec]
      speed /= fps_;

      // pos = this->clips_[clip_id].emulate(step_);

      // send position to servo
      this->servo_driver.setTargetPosition(
          this->joint_servo_map.get(joint_name), pos);
      last_written_positions_[i] = pos;
    }
  }
}
}  // namespace botamochi
#endif