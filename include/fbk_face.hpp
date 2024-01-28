#ifndef FACES_FBKFACE_HPP_
#define FACES_FBKFACE_HPP_

#include <BoundingRect.h>
#include <DrawContext.h>
#include <Drawable.h>
#include <Face.h>
#include <M5Unified.h>  // TODO(meganetaaan): include only the Sprite function not a whole library

namespace m5avatar {
class FbkEye : public Drawable {
  void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx) {
    // ellipse eye
    static uint32_t eye_height = 69;
    static uint32_t eye_width = 35;

    uint32_t cx = rect.getCenterX();
    uint32_t cy = rect.getCenterY();
    Gaze g = ctx->getGaze();
    ColorPalette *cp = ctx->getColorPalette();
    uint16_t primaryColor =
        ctx->getColorDepth() == 1 ? 1 : cp->get(COLOR_PRIMARY);
    uint16_t backgroundColor =
        ctx->getColorDepth() == 1 ? ERACER_COLOR : cp->get(COLOR_BACKGROUND);

    // offset computed from gaze direction
    uint32_t offsetX = g.getHorizontal() * 8;
    uint32_t offsetY = g.getVertical() * 5;
    float eor = ctx->getEyeOpenRatio();

    if (eor == 0) {
      // eye closed
      spi->fillRect(cx - (eye_width / 2), cy - 2, eye_width, 4, primaryColor);
      return;
    }
    // spi->fillEllipse(cx, cy, 30, 25, primaryColor);
    // spi->fillEllipse(cx, cy, 28, 23, backgroundColor);

    spi->fillEllipse(cx + offsetX, cy + offsetY, eye_width / 2, eye_height / 2,
                     primaryColor);
    // spi->fillEllipse(cx + offsetX - 3, cy + offsetY - 3, 3, 3,
    // backgroundColor);
  }
};

class FbkMouth : public Drawable {
 private:
  uint16_t minWidth;
  uint16_t maxWidth;
  uint16_t minHeight;
  uint16_t maxHeight;

 public:
  FbkMouth() : FbkMouth(80, 80, 15, 30) {}
  FbkMouth(uint16_t minWidth, uint16_t maxWidth, uint16_t minHeight,
           uint16_t maxHeight)
      : minWidth{minWidth},
        maxWidth{maxWidth},
        minHeight{minHeight},
        maxHeight{maxHeight} {}
  void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx) {
    uint16_t primaryColor = ctx->getColorDepth() == 1
                                ? 1
                                : ctx->getColorPalette()->get(COLOR_PRIMARY);
    uint16_t backgroundColor =
        ctx->getColorDepth() == 1
            ? ERACER_COLOR
            : ctx->getColorPalette()->get(COLOR_BACKGROUND);
    uint32_t cx = rect.getCenterX();
    uint32_t cy = rect.getCenterY();
    float openRatio = ctx->getMouthOpenRatio();
    uint32_t h = minHeight + (maxHeight - minHeight) * openRatio;
    uint32_t w = minWidth + (maxWidth - minWidth) * (1 - openRatio);

    // spi->drawArc(cx - dx, cy, 39, 2, 1, 2, primaryColor);
    // if (h > minHeight) {
    //   spi->fillEllipse(cx, cy, w / 2, h / 2, primaryColor);
    //   spi->fillEllipse(cx, cy, w / 2 - 4, h / 2 - 4, TFT_RED);
    //   spi->fillRect(cx - w / 2, cy - h / 2, w, h / 2, backgroundColor);
    // }
    // cy = 214
    // spi->fillEllipse(cx, cy - 15, 10, 6, primaryColor); #nose?

    // inner mouse
    spi->fillEllipse(cx, cy - maxHeight / 2, maxWidth / 4,
                     static_cast<int32_t>(maxHeight * openRatio), primaryColor);

    // omega
    spi->fillEllipse(cx - 16, cy - maxHeight / 2, 20, 15,
                     primaryColor);  // outer
    spi->fillEllipse(cx + 16, cy - maxHeight / 2, 20, 15, primaryColor);
    spi->fillEllipse(cx - 16, cy - maxHeight / 2, 18, 13,
                     backgroundColor);  // inner
    spi->fillEllipse(cx + 16, cy - maxHeight / 2, 18, 13, backgroundColor);
    // mask for omega
    spi->fillRect(cx - maxWidth / 2, cy - maxHeight * 1.5, maxWidth, maxHeight,
                  backgroundColor);

    // cheek
    spi->fillEllipse(cx - 132, cy - 23, 24, 10, COLOR_SECONDARY);
    spi->fillEllipse(cx + 132, cy - 23, 24, 10, COLOR_SECONDARY);
  }
};

class FbkFace : public Face {
 public:
  FbkFace()
      : Face(new FbkMouth(), new BoundingRect(225, 160),
             // right eye, second eye arg is center position of eye
             new FbkEye(), new BoundingRect(165, 84),
             //  left eye
             new FbkEye(), new BoundingRect(165, 84 + 154),
             //  hide eye blows with setting these height zero
             new Eyeblow(15, 0, false), new BoundingRect(67, 96),
             new Eyeblow(15, 0, true), new BoundingRect(72, 230)) {}
};

}  // namespace m5avatar

#endif  // FACES_FBKFACE_HPP_
