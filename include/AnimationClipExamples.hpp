#ifndef ANIMATION_CLIP_EXAMPLES_HPP
#define ANIMATION_CLIP_EXAMPLES_HPP
#include "Animation.hpp"

namespace stackchan::motion {
/* Angle unit: 10 = 1 degrees, Speed range: 0~1000 */
/* Range X: -1280 ~ 1280 (-128° ~ 128°), Range Y: 0 ~ 900 (0° ~ 90°) */
AnimationClip look_front_clip((JointName[]){JointName::kHeadPan,
                                            JointName::kHeadTilt},
                              (int16_t[][ANIM_BUFF_LENGTH]){
                                  {IDLE_POSITION, IDLE_POSITION},   // pan
                                  {IDLE_POSITION, IDLE_POSITION}},  // tilt
                              (unsigned short[]){
                                  0,
                                  10,
                              },
                              2);

AnimationClip look_up_clip((JointName[]){JointName::kHeadPan,
                                         JointName::kHeadTilt},
                           (int16_t[][ANIM_BUFF_LENGTH]){
                               {IDLE_POSITION, IDLE_POSITION},         // pan
                               {IDLE_POSITION, IDLE_POSITION + 600}},  // tilt
                           (unsigned short[]){
                               0,
                               10,
                           },
                           2);

AnimationClip look_down_clip((JointName[]){JointName::kHeadPan,
                                           JointName::kHeadTilt},
                             (int16_t[][ANIM_BUFF_LENGTH]){
                                 {IDLE_POSITION, IDLE_POSITION},         // pan
                                 {IDLE_POSITION, IDLE_POSITION - 200}},  // tilt
                             (unsigned short[]){
                                 0,
                                 10,
                             },
                             2);
AnimationClip look_left_clip((JointName[]){JointName::kHeadPan,
                                           JointName::kHeadTilt},
                             (int16_t[][ANIM_BUFF_LENGTH]){
                                 {IDLE_POSITION, (IDLE_POSITION - 600)},  // pan
                                 {IDLE_POSITION, IDLE_POSITION}},  // tilt
                             (unsigned short[]){
                                 0,
                                 10,
                             },
                             2);
AnimationClip look_right_clip((JointName[]){JointName::kHeadPan,
                                            JointName::kHeadTilt},
                              (int16_t[][ANIM_BUFF_LENGTH]){
                                  {IDLE_POSITION, IDLE_POSITION + 600},  // pan
                                  {IDLE_POSITION, IDLE_POSITION}},       // tilt
                              (unsigned short[]){
                                  0,
                                  10,
                              },
                              2);

AnimationClip nod_clip((JointName[]){JointName::kHeadPan, JointName::kHeadTilt},
                       (int16_t[][ANIM_BUFF_LENGTH]){
                           {IDLE_POSITION, IDLE_POSITION, IDLE_POSITION,
                            IDLE_POSITION},  // pan
                           {IDLE_POSITION,
                            IDLE_POSITION + 600,             // slightly look up
                            IDLE_POSITION, IDLE_POSITION}},  // tilt
                       (unsigned short[]){0, 10, 20, 30}, 4);

// NOTE https://qiita.com/dojyorin/items/4bf068aef2b248f1306e
AnimationClip head_shake_clip(
    (JointName[]){JointName::kHeadPan, JointName::kHeadTilt},
    (int16_t[][ANIM_BUFF_LENGTH]){
        {IDLE_POSITION, IDLE_POSITION - 600, IDLE_POSITION + 600,
         IDLE_POSITION},
        {IDLE_POSITION, IDLE_POSITION, IDLE_POSITION, IDLE_POSITION}},
    (unsigned short[]){0, 10, 20, 30}, 4);

}  // namespace stackchan::motion

#endif