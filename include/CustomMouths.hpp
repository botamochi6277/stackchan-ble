#ifndef CUSTOM_MOUTHS_HPP_
#define CUSTOM_MOUTHS_HPP_

#include <BoundingRect.h>
#include <DrawContext.h>
#include <Drawable.h>
#include <Face.h>
// #include <M5Unified.h>  // TODO(botamochi): include only the Sprite function
// not a whole library

namespace m5avatar {

class BaseMouth : public Drawable {
   protected:
    uint16_t min_width_;
    uint16_t max_width_;
    uint16_t min_height_;
    uint16_t max_height_;

    // cache for drawing
    int16_t center_x_;
    int16_t center_y_;
    uint16_t primary_color_;
    uint16_t secondary_color_;
    uint16_t background_color_;
    float open_ratio_;
    Expression expression_;

   public:
    BaseMouth() : BaseMouth(80, 80, 15, 30) {}
    BaseMouth(uint16_t min_width, uint16_t max_width, uint16_t min_height,
              uint16_t max_height)
        : min_width_{min_width},
          max_width_{max_width},
          min_height_{min_height},
          max_height_{max_height} {}

    void update(M5Canvas *spi, BoundingRect rect, DrawContext *ctx) {
        primary_color_ = ctx->getColorDepth() == 1
                             ? 1
                             : ctx->getColorPalette()->get(COLOR_PRIMARY);
        background_color_ = ctx->getColorDepth() == 1
                                ? ERACER_COLOR
                                : ctx->getColorPalette()->get(COLOR_BACKGROUND);
        secondary_color_ = ctx->getColorDepth() == 1
                               ? 1
                               : ctx->getColorPalette()->get(COLOR_SECONDARY);
        center_x_ = rect.getCenterX();
        center_y_ = rect.getCenterY();
        open_ratio_ = ctx->getMouthOpenRatio();
    }
};

class OmegaMouth : public BaseMouth {
   public:
    using BaseMouth::BaseMouth;
    void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx) {
        this->update(spi, rect, ctx);  // update drawing cache
        uint32_t h = min_height_ + (max_height_ - min_height_) * open_ratio_;
        uint32_t w = min_width_ + (max_width_ - min_width_) * (1 - open_ratio_);

        // inner mouse
        spi->fillEllipse(center_x_, center_y_ - max_height_ / 2, max_width_ / 4,
                         static_cast<int32_t>(max_height_ * open_ratio_),
                         primary_color_);

        // omega
        spi->fillEllipse(center_x_ - 16, center_y_ - max_height_ / 2, 20, 15,
                         primary_color_);  // outer
        spi->fillEllipse(center_x_ + 16, center_y_ - max_height_ / 2, 20, 15,
                         primary_color_);
        spi->fillEllipse(center_x_ - 16, center_y_ - max_height_ / 2, 18, 13,
                         background_color_);  // inner
        spi->fillEllipse(center_x_ + 16, center_y_ - max_height_ / 2, 18, 13,
                         background_color_);
        // mask for omega
        spi->fillRect(center_x_ - max_width_ / 2, center_y_ - max_height_ * 1.5,
                      max_width_, max_height_, background_color_);

        // cheek
        spi->fillEllipse(center_x_ - 132, center_y_ - 23, 24, 10,
                         secondary_color_);
        spi->fillEllipse(center_x_ + 132, center_y_ - 23, 24, 10,
                         secondary_color_);
    }
};

}  // namespace m5avatar

#endif