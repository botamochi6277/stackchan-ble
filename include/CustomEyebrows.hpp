#ifndef CUSTOM_EYEBROWS_HPP_
#define CUSTOM_EYEBROWS_HPP_

#include <BoundingRect.h>
#include <DrawContext.h>
#include <Drawable.h>

#include "DrawingUtils.hpp"
namespace m5avatar {
class BaseEyebrow : public Drawable {
   protected:
    uint16_t height_;
    uint16_t width_;
    bool is_left_;

    // caches
    uint16_t primary_color_;
    uint16_t secondary_color_;
    uint16_t background_color_;
    int16_t center_x_;
    int16_t center_y_;
    // float open_ratio_;
    Expression expression_;

   public:
    BaseEyebrow(bool is_left) : BaseEyebrow(30, 20, is_left) {}

    BaseEyebrow(uint16_t width, uint16_t height, bool is_left) {
        this->width_ = width;
        this->height_ = height;
        this->is_left_ = is_left;
    }

    void update(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
        // common process for all standard eyebrows
        // update drawing parameters
        ColorPalette *cp = ctx->getColorPalette();
        primary_color_ = ctx->getColorDepth() == 1 ? 1 : cp->get(COLOR_PRIMARY);
        secondary_color_ = ctx->getColorDepth() == 1
                               ? 1
                               : ctx->getColorPalette()->get(COLOR_SECONDARY);
        background_color_ = ctx->getColorDepth() == 1
                                ? ERACER_COLOR
                                : cp->get(COLOR_BACKGROUND);
        center_x_ = rect.getCenterX();
        center_y_ = rect.getCenterY();
    }
};

// Maro Mayu
class EllipseEyebrow : public BaseEyebrow {
   public:
    using BaseEyebrow::BaseEyebrow;

    void draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
        this->update(canvas, rect, ctx);

        canvas->fillEllipse(center_x_, center_y_, this->width_ / 2,
                            this->height_ / 2, primary_color_);
    }
};

class BowEyebrow : public BaseEyebrow {
   public:
    using BaseEyebrow::BaseEyebrow;

    void draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
        this->update(canvas, rect, ctx);
        uint8_t thickness = 4;

        float angle0 = is_left_ ? 180.0f + 35.0f : 180.0f + 45.0f;
        float stroke_angle = 100.0f;
        canvas->fillArc(center_x_, center_y_, width_ / 2,
                        width_ / 2 - thickness, angle0, angle0 + stroke_angle,
                        primary_color_);
    }
};

}  // namespace m5avatar

#endif