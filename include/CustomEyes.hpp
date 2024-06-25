#ifndef CUSTOM_EYES_HPP_
#define CUSTOM_EYES_HPP_

#include <BoundingRect.h>
#include <DrawContext.h>
#include <Drawable.h>
#include <Face.h>
// #include <M5Unified.h>  // TODO(botamochi): include only the Sprite function
// not a whole library

namespace m5avatar {
class EllipseEye : public Drawable {
   private:
    uint16_t height_;
    uint16_t width_;

   public:
    EllipseEye() : EllipseEye(69, 35) {}
    EllipseEye(uint16_t width, uint16_t height) {
        this->width_ = width;
        this->height_ = height;
    }

    void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx) {
        uint32_t cx = rect.getCenterX();
        uint32_t cy = rect.getCenterY();
        Gaze g = ctx->getGaze();
        ColorPalette *cp = ctx->getColorPalette();
        uint16_t primaryColor =
            ctx->getColorDepth() == 1 ? 1 : cp->get(COLOR_PRIMARY);
        uint16_t backgroundColor = ctx->getColorDepth() == 1
                                       ? ERACER_COLOR
                                       : cp->get(COLOR_BACKGROUND);

        // offset computed from gaze direction
        uint32_t offsetX = g.getHorizontal() * 8;
        uint32_t offsetY = g.getVertical() * 5;
        float eor = ctx->getEyeOpenRatio();

        if (eor == 0) {
            // eye closed
            spi->fillRect(cx - (this->width_ / 2), cy - 2, this->width_, 4,
                          primaryColor);
            return;
        }

        spi->fillEllipse(cx + offsetX, cy + offsetY, this->width_ / 2,
                         this->height_ / 2, primaryColor);
        // spi->fillEllipse(cx + offsetX - 3, cy + offsetY - 3, 3, 3,
        // backgroundColor);
    }
};

}  // namespace m5avatar

#endif