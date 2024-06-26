#ifndef CUSTOM_EYES_HPP_
#define CUSTOM_EYES_HPP_

#include <BoundingRect.h>
#include <DrawContext.h>
#include <Drawable.h>


namespace m5avatar {
// TODO: make BaseEye

class EllipseEye : public Drawable {
   private:
    uint16_t height_;
    uint16_t width_;
    bool is_left_;

   public:
    EllipseEye(bool is_left) : EllipseEye(36, 70, is_left) {}
    EllipseEye(uint16_t width, uint16_t height, bool is_left) {
        this->width_ = width;
        this->height_ = height;
        this->is_left_ = is_left;
    }

    void draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
        // TODO: refactor, this is common process for all eyes
        uint32_t cx = rect.getCenterX();
        uint32_t cy = rect.getCenterY();
        Gaze g = this->is_left_ ? ctx->getLeftGaze() : ctx->getRightGaze();
        ColorPalette *cp = ctx->getColorPalette();
        uint16_t primaryColor =
            ctx->getColorDepth() == 1 ? 1 : cp->get(COLOR_PRIMARY);
        uint16_t backgroundColor = ctx->getColorDepth() == 1
                                       ? ERACER_COLOR
                                       : cp->get(COLOR_BACKGROUND);

        // offset computed from gaze direction
        uint32_t offsetX = g.getHorizontal() * 8;
        uint32_t offsetY = g.getVertical() * 5;
        uint32_t x_shifted = cx + offsetX;
        uint32_t y_shifted = cy + offsetY;
        float eor = this->is_left_ ? ctx->getLeftEyeOpenRatio()
                                   : ctx->getRightEyeOpenRatio();
        auto expression = ctx->getExpression();
        // the end of the common process

        if (eor == 0 || expression == Expression::Sleepy) {
            // eye closed
            // NOTE: the center of closed eye is lower than the center of bbox
            canvas->fillRect(x_shifted - (this->width_ / 2),
                             y_shifted - 2 + this->height_ / 4, this->width_, 4,
                             primaryColor);
            return;
        } else if (expression == Expression::Happy) {
            auto wink_base_y = y_shifted + this->height_ / 4;
            uint32_t thickness = 4;
            canvas->fillEllipse(
                x_shifted, wink_base_y + (1 / 8) * this->height_,
                this->width_ / 2, this->height_ / 4 + thickness, primaryColor);
            // mask
            canvas->fillEllipse(
                x_shifted, wink_base_y + (1 / 8) * this->height_ + thickness,
                this->width_ / 2 - thickness, this->height_ / 4 + thickness,
                backgroundColor);
            canvas->fillRect(x_shifted - this->width_ / 2,
                             wink_base_y + thickness / 2, this->width_,
                             this->height_ / 4, backgroundColor);
            return;
        }

        canvas->fillEllipse(x_shifted, y_shifted, this->width_ / 2,
                            this->height_ / 2, primaryColor);

        // ## expression

        // note: you cannot define variable in switch scope
        int x0, y0, x1, y1, x2, y2;
        switch (expression) {
            case Expression::Angry:
                x0 = x_shifted - width_ / 2;
                y0 = y_shifted - height_ / 2;
                x1 = x_shifted + width_ / 2;
                y1 = y0;
                x2 = this->is_left_ ? x0 : x1;
                y2 = y_shifted - height_ / 4;
                canvas->fillTriangle(x0, y0, x1, y1, x2, y2, backgroundColor);
                break;
            case Expression::Sad:
                x0 = x_shifted - width_ / 2;
                y0 = y_shifted - height_ / 2;
                x1 = x_shifted + width_ / 2;
                y1 = y0;
                x2 = this->is_left_ ? x1 : x0;
                y2 = y_shifted - height_ / 4;
                canvas->fillTriangle(x0, y0, x1, y1, x2, y2, backgroundColor);
                break;
            case Expression::Doubt:
                // top left
                x0 = x_shifted - width_ / 2;
                y0 = y_shifted - height_ / 2;
                // bottom right
                x1 = x_shifted + width_ / 2;
                y1 = y_shifted - height_ / 4;

                canvas->fillRect(x0, y0, x1 - x0, y1 - y0, backgroundColor);
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