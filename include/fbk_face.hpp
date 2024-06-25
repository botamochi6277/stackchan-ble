#ifndef FACES_FBKFACE_HPP_
#define FACES_FBKFACE_HPP_

#include <BoundingRect.h>
#include <DrawContext.h>
#include <Drawable.h>
#include <Face.h>
#include <M5Unified.h>  // TODO(meganetaaan): include only the Sprite function not a whole library

#include "CustomEyes.hpp"
#include "CustomMouth.hpp"
namespace m5avatar {

class FbkFace : public Face {
   public:
    FbkFace()
        : Face(new OmegaMouth(), new BoundingRect(225, 160),
               // right eye, second eye arg is center position of eye
               new EllipseEye(), new BoundingRect(165, 84),
               //  left eye
               new EllipseEye(), new BoundingRect(165, 84 + 154),
               //  hide eye blows with setting these height zero
               new Eyeblow(15, 0, false), new BoundingRect(67, 96),
               new Eyeblow(15, 0, true), new BoundingRect(72, 230)) {}
};

}  // namespace m5avatar

#endif  // FACES_FBKFACE_HPP_
