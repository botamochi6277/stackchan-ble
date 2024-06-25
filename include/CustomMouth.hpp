#ifndef CUSTOM_MOUTH_HPP_
#define CUSTOM_MOUTH_HPP_

#include <BoundingRect.h>
#include <DrawContext.h>
#include <Drawable.h>
#include <Face.h>
// #include <M5Unified.h>  // TODO(botamochi): include only the Sprite function
// not a whole library

namespace m5avatar {

class OmegaMouth : public Drawable {
   private:
    uint16_t min_width_;
    uint16_t max_width_;
    uint16_t min_height_;
    uint16_t max_height_;

   public:
    OmegaMouth() : OmegaMouth(80, 80, 15, 30) {}
    OmegaMouth(uint16_t min_width, uint16_t max_width, uint16_t min_height,
               uint16_t max_height)
        : min_width_{min_width},
          max_width_{max_width},
          min_height_{min_height},
          max_height_{max_height} {}

    void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx) {
        uint16_t primaryColor =
            ctx->getColorDepth() == 1
                ? 1
                : ctx->getColorPalette()->get(COLOR_PRIMARY);
        uint16_t backgroundColor =
            ctx->getColorDepth() == 1
                ? ERACER_COLOR
                : ctx->getColorPalette()->get(COLOR_BACKGROUND);
        uint16_t secondary_color =
            ctx->getColorDepth() == 1
                ? 1
                : ctx->getColorPalette()->get(COLOR_SECONDARY);
        uint32_t cx = rect.getCenterX();
        uint32_t cy = rect.getCenterY();
        float openRatio = ctx->getMouthOpenRatio();
        uint32_t h = min_height_ + (max_height_ - min_height_) * openRatio;
        uint32_t w = min_width_ + (max_width_ - min_width_) * (1 - openRatio);

        // inner mouse
        spi->fillEllipse(cx, cy - max_height_ / 2, max_width_ / 4,
                         static_cast<int32_t>(max_height_ * openRatio),
                         primaryColor);

        // omega
        spi->fillEllipse(cx - 16, cy - max_height_ / 2, 20, 15,
                         primaryColor);  // outer
        spi->fillEllipse(cx + 16, cy - max_height_ / 2, 20, 15, primaryColor);
        spi->fillEllipse(cx - 16, cy - max_height_ / 2, 18, 13,
                         backgroundColor);  // inner
        spi->fillEllipse(cx + 16, cy - max_height_ / 2, 18, 13,
                         backgroundColor);
        // mask for omega
        spi->fillRect(cx - max_width_ / 2, cy - max_height_ * 1.5, max_width_,
                      max_height_, backgroundColor);

        // cheek
        spi->fillEllipse(cx - 132, cy - 23, 24, 10, secondary_color);
        spi->fillEllipse(cx + 132, cy - 23, 24, 10, secondary_color);
    }
};

}  // namespace m5avatar

#endif