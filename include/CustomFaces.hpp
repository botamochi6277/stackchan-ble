#ifndef CUSTOM_FACES_HPP_
#define CUSTOM_FACES_HPP_

#include <BoundingRect.h>
#include <DrawContext.h>
#include <Drawable.h>
#include <Face.h>
#include <M5Unified.h>  // TODO(meganetaaan): include only the Sprite function not a whole library

#include "CustomEyes.hpp"
#include "CustomMouths.hpp"
namespace m5avatar {

class FbkFace : public Face {
   public:
    FbkFace()
        : Face(new OmegaMouth(), new BoundingRect(225, 160),
               // right eye, second eye arg is center position of eye
               new EllipseEye(false), new BoundingRect(165, 84),
               //  left eye
               new EllipseEye(true), new BoundingRect(165, 84 + 154),
               //  hide eye brows with setting these height zero
               new Eyeblow(15, 0, false), new BoundingRect(67, 96),
               new Eyeblow(15, 0, true), new BoundingRect(72, 230)) {}
};

}  // namespace m5avatar

#endif  // CUSTOM_FACES_HPP_
