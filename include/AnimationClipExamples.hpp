#ifndef ANIMATION_CLIP_EXAMPLES_HPP
#define ANIMATION_CLIP_EXAMPLES_HPP
#include "Animation.hpp"

namespace botamochi {

AnimationClip look_front_clip((JointName[]){JointName::kHeadPan,
                                            JointName::kHeadTilt},
                              (unsigned short[][ANIM_BUFF_LENGTH]){
                                  {IDLE_POSITION, IDLE_POSITION},   // pan
                                  {IDLE_POSITION, IDLE_POSITION}},  // tilt
                              (unsigned short[]){
                                  0,
                                  10,
                              },
                              2);

AnimationClip look_up_clip((JointName[]){JointName::kHeadPan,
                                         JointName::kHeadTilt},
                           (unsigned short[][ANIM_BUFF_LENGTH]){
                               {IDLE_POSITION, IDLE_POSITION},        // pan
                               {IDLE_POSITION, IDLE_POSITION + 50}},  // tilt
                           (unsigned short[]){
                               0,
                               10,
                           },
                           2);

AnimationClip look_down_clip((JointName[]){JointName::kHeadPan,
                                           JointName::kHeadTilt},
                             (unsigned short[][ANIM_BUFF_LENGTH]){
                                 {IDLE_POSITION, IDLE_POSITION},        // pan
                                 {IDLE_POSITION, IDLE_POSITION - 20}},  // tilt
                             (unsigned short[]){
                                 0,
                                 10,
                             },
                             2);
AnimationClip look_left_clip((JointName[]){JointName::kHeadPan,
                                           JointName::kHeadTilt},
                             (unsigned short[][ANIM_BUFF_LENGTH]){
                                 {IDLE_POSITION, IDLE_POSITION - 100},  // pan
                                 {IDLE_POSITION, IDLE_POSITION}},       // tilt
                             (unsigned short[]){
                                 0,
                                 10,
                             },
                             2);
AnimationClip look_right_clip((JointName[]){JointName::kHeadPan,
                                            JointName::kHeadTilt},
                              (unsigned short[][ANIM_BUFF_LENGTH]){
                                  {IDLE_POSITION, IDLE_POSITION + 100},  // pan
                                  {IDLE_POSITION, IDLE_POSITION}},       // tilt
                              (unsigned short[]){
                                  0,
                                  10,
                              },
                              2);

AnimationClip nod_clip((JointName[]){JointName::kHeadPan, JointName::kHeadTilt},
                       (unsigned short[][ANIM_BUFF_LENGTH]){
                           {IDLE_POSITION, IDLE_POSITION, IDLE_POSITION,
                            IDLE_POSITION},  // pan
                           {IDLE_POSITION,
                            IDLE_POSITION + 50,  // slightly look up
                            IDLE_POSITION - 10, IDLE_POSITION}},  // tilt
                       (unsigned short[]){0, 10, 20, 30}, 4);

// NOTE https://qiita.com/dojyorin/items/4bf068aef2b248f1306e
AnimationClip head_shake_clip(
    (JointName[]){JointName::kHeadPan, JointName::kHeadTilt},
    (unsigned short[][ANIM_BUFF_LENGTH]){
        {IDLE_POSITION, IDLE_POSITION - 100, IDLE_POSITION + 100,
         IDLE_POSITION},
        {IDLE_POSITION, IDLE_POSITION, IDLE_POSITION, IDLE_POSITION}},
    (unsigned short[]){0, 10, 20, 30}, 4);

}  // namespace botamochi

#endif