
#ifndef LEONA_FACE_HPP
#define LEONA_FACE_HPP
#include <Eyes.hpp>
#include <Mouths.hpp>
#include <faces/FaceTemplates.hpp>

namespace m5avatar {

class LeonaFace : public Face {
 public:
  LeonaFace()
      : Face(new OmegaMouth(), new BoundingRect(225, 160),
             // right eye, second eye arg is center position of eye
             new ToonEye2(60, 84, false), new BoundingRect(163, 64),
             //  left eye
             new ToonEye2(60, 84, true), new BoundingRect(163, 256),

             // right eyebrow
             new EllipseEyebrow(36, 0, false),
             new BoundingRect(97 + 10, 84 + 18),  // (y,x)
                                                  //  left eyebrow
             new EllipseEyebrow(36, 0, true), new BoundingRect(107, 200 + 18)) {
  }
};

}  // namespace m5avatar

#endif