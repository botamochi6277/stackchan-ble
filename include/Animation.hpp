
#ifndef BOTAMOCHI_ANIMATION_HPP
#define BOTAMOCHI_ANIMATION_HPP

#define ANIM_BUFF_LENGTH 8
#define IDLE_POSITION 511
#define MAX_ANIM_CLIP_NUM 8
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

class AnimationClip {
 private:
  /* data */
  JointName joint_name_;
  unsigned short positions_[ANIM_BUFF_LENGTH];
  unsigned short speeds_[ANIM_BUFF_LENGTH];
  unsigned short keyframes_[ANIM_BUFF_LENGTH];
  unsigned short length_;    // num values of the clip
  unsigned short duration_;  // in frames
  unsigned int last_play_frame_id_;
  unsigned short num_played_;

 public:
  AnimationClip();
  AnimationClip(JointName joint_name, unsigned short positions[],
                unsigned short keyframes[], unsigned short length);
  ~AnimationClip();
  inline JointName getJointName() { return joint_name_; }
  void setWaypoints(unsigned short positions[], unsigned short keyframes[],
                    unsigned short length);
  bool isPlaying(unsigned short current_frame_id);
  void play(unsigned short current_frame_id);

  unsigned short emulate(unsigned short current_frame_id);

  void emulate_position_and_speed(unsigned short current_frame_id,
                                  unsigned short& position,
                                  unsigned short& speed);
};

AnimationClip::AnimationClip()
    : joint_name_(JointName::kHeadPan),
      positions_({IDLE_POSITION, IDLE_POSITION}),
      speeds_({IDLE_POSITION, IDLE_POSITION}),
      keyframes_({0, 10}),
      length_(2),
      last_play_frame_id_(0) {
  duration_ = 10;
  num_played_ = 0;
}
AnimationClip::AnimationClip(JointName joint_name, unsigned short positions[],
                             unsigned short keyframes[],
                             unsigned short length) {
  for (unsigned short i = 0; i < length; i++) {
    if (length >= ANIM_BUFF_LENGTH) {
      break;
    }

    this->positions_[i] = positions[i];
    this->keyframes_[i] = keyframes[i];
    if (i == 0) {
      this->speeds_[i] = positions[i + 1] > positions[0]
                             ? (positions[i + 1] - positions[0])
                             : (positions[0] - positions[i + 1]);
    } else if (i == length - 1) {
      this->speeds_[i] = positions[i] > positions[i - 1]
                             ? (positions[i] - positions[i - 1])
                             : (positions[i - 1] - positions[i]);
    } else {
      this->speeds_[i] = positions[i + 1] > positions[i - 1]
                             ? (positions[i + 1] - positions[i - 1]) / 2
                             : (positions[i - 1] - positions[i + 1]) / 2;
    }
  }
  joint_name_ = joint_name;
  length_ = length;
  last_play_frame_id_ = 0;  // will start playing from 1
  num_played_ = 0;
  duration_ = keyframes[length_ - 1] - keyframes[0];
}

void AnimationClip::setWaypoints(unsigned short positions[],
                                 unsigned short keyframes[],
                                 unsigned short length) {
  for (unsigned short i = 0; i < length; i++) {
    if (length >= ANIM_BUFF_LENGTH) {
      break;
    }

    this->positions_[i] = positions[i];
    this->keyframes_[i] = keyframes[i];
  }
  length_ = length;
}

bool AnimationClip::isPlaying(unsigned short current_frame_id) {
  if (num_played_ == 0) {
    return false;
  }
  // last_play_frame_id_ <= current_frame_id < last_play_frame_id_ + duration_

  return ((last_play_frame_id_ <= current_frame_id) &&
          (current_frame_id < last_play_frame_id_ + duration_));
}

void AnimationClip::play(unsigned short current_frame_id) {
  num_played_ += 1;
  last_play_frame_id_ = current_frame_id;
};

unsigned short AnimationClip::emulate(unsigned short current_frame_id) {
  if (this->last_play_frame_id_ > current_frame_id) {
    return positions_[0];
  }

  unsigned short t = current_frame_id - this->last_play_frame_id_;
  if (t > this->keyframes_[length_ - 1]) {
    return positions_[this->length_ - 1];
  }

  for (unsigned short i = 1; i < this->length_; i++) {
    // find i meeting (keyframes[i-1]< t < keyframes[i])
    if (t < keyframes_[i - 1] || keyframes_[i] < t) {
      continue;
    }

    return remap(t, keyframes_[i - 1], keyframes_[i], positions_[i - 1],
                 positions_[i]);
  }

  return positions_[0];
}

void AnimationClip::emulate_position_and_speed(unsigned short current_frame_id,
                                               unsigned short& position,
                                               unsigned short& speed) {
  // before initial keyframe
  if (this->last_play_frame_id_ > current_frame_id) {
    position = positions_[0];
    speed = speeds_[0];
    return;
  }

  unsigned short t = current_frame_id - this->last_play_frame_id_;
  // after last keyframe
  if (t > this->keyframes_[length_ - 1]) {
    position = positions_[this->length_ - 1];
    speed = speeds_[this->length_ - 1];
    return;
  }

  for (unsigned short i = 1; i < this->length_; i++) {
    // find i meeting (keyframes[i-1] < t < keyframes[i])
    if (t < keyframes_[i - 1] || keyframes_[i] < t) {
      continue;
    }
    // interpolate
    position = remap(t, keyframes_[i - 1], keyframes_[i], positions_[i - 1],
                     positions_[i]);
    speed =
        remap(t, keyframes_[i - 1], keyframes_[i], speeds_[i - 1], speeds_[i]);

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

AnimationController::AnimationController(unsigned short fps) : fps_(fps) {}

AnimationController::~AnimationController() {}

void AnimationController::setClip(unsigned short id, AnimationClip& clip) {
  this->clips_[id] = clip;
}

void AnimationController::play(unsigned short clip_id) {
  // play from current frame, set playing frame_id
  this->clips_[clip_id].play(step_);
}

void AnimationController::update() {
  step_ += 1;  // start from 1
  unsigned short pos;
  unsigned short speed;
  for (unsigned short clip_id = 0; clip_id < MAX_ANIM_CLIP_NUM; clip_id++) {
    if (!this->clips_[clip_id].isPlaying(step_)) {
      continue;
    }

    this->clips_[clip_id].emulate_position_and_speed(step_, pos, speed);

    // speed [pulse/frame] -> [pulse/sec]
    speed /= fps_;

    // pos = this->clips_[clip_id].emulate(step_);

    // send position to servo
    this->servo_driver.setTargetPosition(
        this->joint_servo_map.get(this->clips_[clip_id].getJointName()), pos);
  }
}
}  // namespace botamochi
#endif