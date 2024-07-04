#ifndef CUSTOM_EYES_HPP_
#define CUSTOM_EYES_HPP_

#include <BoundingRect.h>
#include <DrawContext.h>
#include <Drawable.h>

namespace m5avatar {
// TODO: make BaseEye

class BaseEye : public Drawable {
   protected:
    uint16_t height_;
    uint16_t width_;
    bool is_left_;

    // cache for drawing
    int16_t center_x_;
    int16_t center_y_;
    Gaze gaze_;
    uint16_t primary_color_;
    uint16_t secondary_color_;
    uint16_t background_color_;
    int16_t shifted_x_;
    int16_t shifted_y_;
    float open_ratio_;
    Expression expression_;

   public:
    BaseEye(bool is_left) : BaseEye(36, 70, is_left) {}
    BaseEye(uint16_t width, uint16_t height, bool is_left) {
        this->width_ = width;
        this->height_ = height;
        this->is_left_ = is_left;
    }
    void update(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
        // common process for all standard eyes
        // update drawing parameters
        center_x_ = rect.getCenterX();
        center_y_ = rect.getCenterY();
        gaze_ = this->is_left_ ? ctx->getLeftGaze() : ctx->getRightGaze();
        ColorPalette *cp = ctx->getColorPalette();
        primary_color_ = ctx->getColorDepth() == 1 ? 1 : cp->get(COLOR_PRIMARY);
        secondary_color_ = ctx->getColorDepth() == 1
                               ? 1
                               : ctx->getColorPalette()->get(COLOR_SECONDARY);
        background_color_ = ctx->getColorDepth() == 1
                                ? ERACER_COLOR
                                : cp->get(COLOR_BACKGROUND);

        // offset computed from gaze direction
        shifted_x_ = center_x_ + gaze_.getHorizontal() * 8;
        shifted_y_ = center_y_ + gaze_.getVertical() * 5;
        open_ratio_ = this->is_left_ ? ctx->getLeftEyeOpenRatio()
                                     : ctx->getRightEyeOpenRatio();
        expression_ = ctx->getExpression();
    }
};

class EllipseEye : public BaseEye {
   public:
    using BaseEye::BaseEye;
    void draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
        this->update(canvas, rect, ctx);
        if (open_ratio_ == 0 || expression_ == Expression::Sleepy) {
            // eye closed
            // NOTE: the center of closed eye is lower than the center of bbox
            canvas->fillRect(shifted_x_ - (this->width_ / 2),
                             shifted_y_ - 2 + this->height_ / 4, this->width_,
                             4, primary_color_);
            return;
        } else if (expression_ == Expression::Happy) {
            auto wink_base_y = shifted_y_ + this->height_ / 4;
            uint32_t thickness = 4;
            canvas->fillEllipse(shifted_x_,
                                wink_base_y + (1 / 8) * this->height_,
                                this->width_ / 2, this->height_ / 4 + thickness,
                                primary_color_);
            // mask
            canvas->fillEllipse(
                shifted_x_, wink_base_y + (1 / 8) * this->height_ + thickness,
                this->width_ / 2 - thickness, this->height_ / 4 + thickness,
                background_color_);
            canvas->fillRect(shifted_x_ - this->width_ / 2,
                             wink_base_y + thickness / 2, this->width_,
                             this->height_ / 4, background_color_);
            return;
        }

        canvas->fillEllipse(shifted_x_, shifted_y_, this->width_ / 2,
                            this->height_ / 2, primary_color_);

        // note: you cannot define variable in switch scope
        int x0, y0, x1, y1, x2, y2;
        switch (expression_) {
            case Expression::Angry:
                x0 = shifted_x_ - width_ / 2;
                y0 = shifted_y_ - height_ / 2;
                x1 = shifted_x_ + width_ / 2;
                y1 = y0;
                x2 = this->is_left_ ? x0 : x1;
                y2 = shifted_y_ - height_ / 4;
                canvas->fillTriangle(x0, y0, x1, y1, x2, y2, background_color_);
                break;
            case Expression::Sad:
                x0 = shifted_x_ - width_ / 2;
                y0 = shifted_y_ - height_ / 2;
                x1 = shifted_x_ + width_ / 2;
                y1 = y0;
                x2 = this->is_left_ ? x1 : x0;
                y2 = shifted_y_ - height_ / 4;
                canvas->fillTriangle(x0, y0, x1, y1, x2, y2, background_color_);
                break;
            case Expression::Doubt:
                // top left
                x0 = shifted_x_ - width_ / 2;
                y0 = shifted_y_ - height_ / 2;
                // bottom right
                x1 = shifted_x_ + width_ / 2;
                y1 = shifted_y_ - height_ / 4;

                canvas->fillRect(x0, y0, x1 - x0, y1 - y0, background_color_);
                break;
            case Expression::Sleepy:
                break;

            default:
                break;
        }
    }
};

class GirlyEye : public BaseEye {
   public:
    using BaseEye::BaseEye;
    void draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
        this->update(canvas, rect, ctx);
        auto wink_base_y =
            shifted_y_ + (1.0f - open_ratio_) * this->height_ / 4;

        // eyelashes
        uint16_t el_x0, el_y0, el_x1, el_y1, el_x2, el_y2;
        el_x0 = this->is_left_ ? shifted_x_ + 22 : shifted_x_ - 22;
        el_y0 = wink_base_y - 32 * open_ratio_ - 27;
        el_x1 = this->is_left_ ? shifted_x_ + 26 : shifted_x_ - 26;
        el_y1 = wink_base_y - 32 * open_ratio_;
        el_x2 = this->is_left_ ? shifted_x_ - 10 : shifted_x_ + 10;
        el_y2 = wink_base_y - 32 * open_ratio_;
        canvas->fillTriangle(el_x0, el_y0, el_x1, el_y1, el_x2, el_y2,
                             primary_color_);
        uint32_t thickness = 4;
        if (open_ratio_ == 0 || expression_ == Expression::Sleepy) {
            // eye closed
            // NOTE: the center of closed eye is lower than the center of bbox
            canvas->fillRect(shifted_x_ - (this->width_ / 2), wink_base_y - 2,
                             this->width_, 4, primary_color_);
            return;
        } else if (expression_ == Expression::Happy) {
            canvas->fillEllipse(shifted_x_,
                                wink_base_y + (1 / 8) * this->height_,
                                this->width_ / 2, this->height_ / 4 + thickness,
                                primary_color_);
            // mask
            canvas->fillEllipse(
                shifted_x_, wink_base_y + (1 / 8) * this->height_ + thickness,
                this->width_ / 2 - thickness, this->height_ / 4 + thickness,
                background_color_);
            canvas->fillRect(shifted_x_ - this->width_ / 2,
                             wink_base_y + thickness / 2, this->width_,
                             this->height_ / 4, background_color_);
            return;
        }
        // canvas->fillArc
        // bg
        canvas->fillEllipse(shifted_x_, shifted_y_, this->width_ / 2,
                            this->height_ / 2, primary_color_);
        canvas->fillEllipse(shifted_x_, shifted_y_,
                            this->width_ / 2 - thickness,
                            this->height_ / 2 - thickness, secondary_color_);
        // upper half moon
        canvas->fillArc(shifted_x_, shifted_y_, width_ / 2, 0, 180.0f, 360.0f,
                        primary_color_);

        canvas->fillEllipse(shifted_x_, shifted_y_, this->width_ / 4,
                            this->height_ / 4, primary_color_);
        // high light
        canvas->fillEllipse(shifted_x_ - width_ / 6, shifted_y_ - height_ / 6,
                            width_ / 6, height_ / 6, 0xffffff);
        // note: you cannot define variable in switch scope
        int x0, y0, x1, y1, x2, y2;
        switch (expression_) {
            case Expression::Angry:
                x0 = shifted_x_ - width_ / 2;
                y0 = shifted_y_ - height_ / 2;
                x1 = shifted_x_ + width_ / 2;
                y1 = y0;
                x2 = this->is_left_ ? x0 : x1;
                y2 = shifted_y_ - height_ / 4;
                canvas->fillTriangle(x0, y0, x1, y1, x2, y2, background_color_);
                break;
            case Expression::Sad:
                x0 = shifted_x_ - width_ / 2;
                y0 = shifted_y_ - height_ / 2;
                x1 = shifted_x_ + width_ / 2;
                y1 = y0;
                x2 = this->is_left_ ? x1 : x0;
                y2 = shifted_y_ - height_ / 4;
                canvas->fillTriangle(x0, y0, x1, y1, x2, y2, background_color_);
                break;
            case Expression::Doubt:
                // top left
                x0 = shifted_x_ - width_ / 2;
                y0 = shifted_y_ - height_ / 2;
                // bottom right
                x1 = shifted_x_ + width_ / 2;
                y1 = shifted_y_ - height_ / 4;

                canvas->fillRect(x0, y0, x1 - x0, y1 - y0, background_color_);
                break;
            case Expression::Sleepy:

                break;

            default:
                break;
        }
    }
};

}  // namespace m5avatar

#endif