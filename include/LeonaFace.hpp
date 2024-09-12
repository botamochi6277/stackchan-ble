
#ifndef LEONA_FACE_HPP
#define LEONA_FACE_HPP
#include <Eyes.hpp>
#include <Mouths.hpp>
#include <faces/FaceTemplates.hpp>

namespace m5avatar {

class NuiEye : public BaseEye {
   public:
    using BaseEye::BaseEye;
    void drawEyelid(M5Canvas *canvas);
    void drawEyelash(M5Canvas *canvas);
    /**
     * @brief draw eyelash at 100% open ratio
     *
     * @param canvas
     */
    void drawEyelash100(M5Canvas *canvas);

    void overwriteOpenRatio();
    void draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx);
};

void NuiEye::drawEyelid(M5Canvas *canvas) {
    // eyelid
    auto upper_eyelid_y = shifted_y_ - 0.8f * height_ / 2 +
                          (1.0f - open_ratio_) * this->height_ * 0.6;

    float eyelash_x0, eyelash_y0, eyelash_x1, eyelash_y1, eyelash_x2,
        eyelash_y2;
    eyelash_x0 = this->is_left_ ? shifted_x_ + 22 : shifted_x_ - 22;
    eyelash_y0 = upper_eyelid_y - 27;
    eyelash_x1 = this->is_left_ ? shifted_x_ + 26 : shifted_x_ - 26;
    eyelash_y1 = upper_eyelid_y;
    eyelash_x2 = this->is_left_ ? shifted_x_ - 10 : shifted_x_ + 10;
    eyelash_y2 = upper_eyelid_y;

    float tilt = 0.0f;
    float ref_tilt = open_ratio_ * M_PI / 6.0f;
    float bias;
    if (expression_ == Expression::Angry) {
        tilt = this->is_left_ ? -ref_tilt : ref_tilt;
    } else if (expression_ == Expression::Sad) {
        tilt = this->is_left_ ? ref_tilt : -ref_tilt;
    }
    bias = 0.2f * width_ * tilt / (M_PI / 6.0f);

    if ((open_ratio_ < 0.99f) || (abs(tilt) > 0.1f)) {
        // mask
        // top:shifted_y_ - this->height_ / 2
        // bottom: upper_eyelid_y
        float mask_top_left_x = shifted_x_ - (this->width_ / 2);
        float mask_top_left_y = shifted_y_ - 0.75f * this->height_;
        float mask_bottom_right_x = shifted_x_ + (this->width_ / 2);
        float mask_bottom_right_y = upper_eyelid_y;

        fillRectRotatedAround(canvas, mask_top_left_x, mask_top_left_y,
                              mask_bottom_right_x, mask_bottom_right_y, tilt,
                              shifted_x_, upper_eyelid_y, background_color_);

        // eyelid
        float eyelid_top_left_x = shifted_x_ - (this->width_ / 2) + bias;
        float eyelid_top_left_y = upper_eyelid_y - 4;
        float eyelid_bottom_right_x = shifted_x_ + (this->width_ / 2) + bias;
        float eyelid_bottom_right_y = upper_eyelid_y;

        fillRectRotatedAround(canvas, eyelid_top_left_x, eyelid_top_left_y,
                              eyelid_bottom_right_x, eyelid_bottom_right_y,
                              tilt, shifted_x_, upper_eyelid_y, primary_color_);

        eyelash_x0 += bias;
        eyelash_x1 += bias;
        eyelash_x2 += bias;
    }

    // eyelash
    rotatePointAround(eyelash_x0, eyelash_y0, tilt, shifted_x_, upper_eyelid_y);
    rotatePointAround(eyelash_x1, eyelash_y1, tilt, shifted_x_, upper_eyelid_y);
    rotatePointAround(eyelash_x2, eyelash_y2, tilt, shifted_x_, upper_eyelid_y);
    canvas->fillTriangle(eyelash_x0, eyelash_y0, eyelash_x1, eyelash_y1,
                         eyelash_x2, eyelash_y2, primary_color_);
}

void NuiEye::drawEyelash100(M5Canvas *canvas) {
    // auto upper_eyelid_y = shifted_y_ - 0.8f * height_ / 2 +
    //                       (1.0f - open_ratio_) * this->height_ * 0.6;
    // y position in eye coordinates
    float upper_eyelid_y = (-0.8f * (height_ / 2)) * (open_ratio_);
    float eyelash_x0, eyelash_y0, eyelash_x1, eyelash_y1, eyelash_x2,
        eyelash_y2;

    // TODO: re-define eyelash size according to eye_size

    float rx = static_cast<float>(width_ / 2);
    float ry = static_cast<float>(height_ / 2);
    // (x0,y0) is on the ellipse
    eyelash_y0 = upper_eyelid_y;
    eyelash_x0 = rx * sqrtf(1.0f - powf(eyelash_y0 / (ry + 1e-9f), 2));

    // (x1,y1) is on the ellipse
    eyelash_y1 = upper_eyelid_y - ry * 0.15f;
    eyelash_x1 = rx * sqrtf(1.0f - powf(eyelash_y1 / (ry + 1e-9f), 2));

    eyelash_y2 = upper_eyelid_y - 0.5f * ry;
    eyelash_x2 = rx;

    // mirroring
    if (!is_left_) {
        eyelash_x0 *= -1.0f;
        eyelash_x1 *= -1.0f;
        eyelash_x2 *= -1.0f;
    }

    // convert position to display coordinates

    eyelash_x0 += shifted_x_;
    eyelash_x1 += shifted_x_;
    eyelash_x2 += shifted_x_;

    eyelash_y0 += shifted_y_;
    eyelash_y1 += shifted_y_;
    eyelash_y2 += shifted_y_;

    float tilt = 0.0f;
    float ref_tilt = open_ratio_ * M_PI / 6.0f;
    float bias;
    if (expression_ == Expression::Angry) {
        tilt = this->is_left_ ? -ref_tilt : ref_tilt;
    } else if (expression_ == Expression::Sad) {
        tilt = this->is_left_ ? ref_tilt : -ref_tilt;
    }
    bias = 0.2f * width_ * tilt / (M_PI / 6.0f);
    if ((open_ratio_ < 0.99f) || (abs(tilt) > 0.1f)) {
        eyelash_x0 += bias;
        eyelash_x1 += bias;
        eyelash_x2 += bias;

        rotatePointAround(eyelash_x0, eyelash_y0, tilt, shifted_x_,
                          upper_eyelid_y);
        rotatePointAround(eyelash_x1, eyelash_y1, tilt, shifted_x_,
                          upper_eyelid_y);
        rotatePointAround(eyelash_x2, eyelash_y2, tilt, shifted_x_,
                          upper_eyelid_y);
    }

    canvas->fillTriangle(eyelash_x0, eyelash_y0, eyelash_x1, eyelash_y1,
                         eyelash_x2, eyelash_y2, primary_color_);
}

void NuiEye::overwriteOpenRatio() {
    switch (expression_) {
        case Expression::Doubt:
            open_ratio_ = 0.6f;
            break;

        case Expression::Sleepy:
            open_ratio_ = 0.0f;
            break;
    }
}

void NuiEye::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
    this->update(canvas, rect, ctx);
    this->overwriteOpenRatio();
    auto wink_base_y = shifted_y_ + (1.0f - open_ratio_) * this->height_ / 4;

    uint32_t thickness = 4;
    if (expression_ == Expression::Happy) {
        canvas->fillEllipse(shifted_x_, wink_base_y + (1 / 8) * this->height_,
                            this->width_ / 2, this->height_ / 4 + thickness,
                            primary_color_);
        // mask
        canvas->fillEllipse(shifted_x_,
                            wink_base_y + (1 / 8) * this->height_ + thickness,
                            this->width_ / 2 - thickness,
                            this->height_ / 4 + thickness, background_color_);
        canvas->fillRect(shifted_x_ - this->width_ / 2,
                         wink_base_y + thickness / 2, this->width_,
                         this->height_ / 4, background_color_);
        // this->drawEyelid(canvas);
        return;
    }
    // main eye
    if (open_ratio_ > 0.1f) {
        // bg
        canvas->fillEllipse(shifted_x_, shifted_y_, this->width_ / 2,
                            this->height_ / 2, primary_color_);

        uint16_t iris_color_2 = M5.Lcd.color24to16(0xFCA145);
        canvas->fillEllipse(shifted_x_, shifted_y_,
                            this->width_ / 2 - thickness,
                            this->height_ / 2 - thickness, iris_color_2);
        // center line
        uint16_t iris_color_1 = M5.Lcd.color24to16(0x73463B);
        canvas->fillRect(shifted_x_ - width_ / 2, shifted_y_, width_, 1,
                         iris_color_1);

        // upper half moon wil fill
        canvas->floodFill(shifted_x_, shifted_y_ - 2, iris_color_1);

        // pupil
        // canvas->fillEllipse(shifted_x_, shifted_y_, this->width_ / 4,
        //                     this->height_ / 4, primary_color_);

        // high light
        canvas->fillEllipse(shifted_x_ - width_ / 6, shifted_y_ - height_ / 6,
                            width_ / 8, height_ / 8, 0xffffff);
    }
    if (this->open_ratio_ < 0.99f || this->expression_ != Expression::Neutral) {
        this->drawEyelid(canvas);
    } else {
        this->drawEyelash100(canvas);
    }
}

class LeonaFace : public Face {
   public:
    LeonaFace()
        : Face(new OmegaMouth(), new BoundingRect(225, 160),
               // right eye, second eye arg is center position of eye
               new NuiEye(60, 84, false), new BoundingRect(163, 64),
               //  left eye
               new NuiEye(60, 84, true), new BoundingRect(163, 256),

               // right eyebrow
               new EllipseEyebrow(36, 0, false),
               new BoundingRect(97 + 10, 84 + 18),  // (y,x)
                                                    //  left eyebrow
               new EllipseEyebrow(36, 0, true),
               new BoundingRect(107, 200 + 18)) {}
};

}  // namespace m5avatar

#endif