#ifndef PALETTE_EXAMPLES_HPP
#define PALETTE_EXAMPLES_HPP
#include <ColorPalette.h>

namespace m5avatar {

void assignUiPalette(ColorPalette* palette) {
  palette->set(DrawingLocation::kIris1,
               M5.Lcd.color24to16(0x325165));  // iris 1, #325165
  palette->set(DrawingLocation::kEyelash, M5.Lcd.color24to16(0x325165));
  palette->set(DrawingLocation::kEyelid,
               M5.Lcd.color24to16(0x325170));  // #325170
  palette->set(DrawingLocation::kIrisBackground, TFT_BLACK);
  palette->set(DrawingLocation::kIris2,
               M5.Lcd.color24to16(0xD8F2B3));  // iris2, #D8F2B3
  palette->set(DrawingLocation::kPupil,
               M5.Lcd.color24to16(0x2D493A));  // #2D493A
  palette->set(DrawingLocation::kEyeHighlight, TFT_WHITE);
  palette->set(DrawingLocation::kSkin,
               M5.Lcd.color24to16(0xfac2a8));  // skin
  palette->set(DrawingLocation::kEyeBrow, M5.Lcd.color24to16(0x321F39));
  palette->set(DrawingLocation::kMouthBackground, TFT_BLACK);
  palette->set(DrawingLocation::kCheek1,
               TFT_PINK);  // cheek
}

void assignFbkPalette(ColorPalette* palette) {
  palette->set(DrawingLocation::kIrisBackground,
               M5.Lcd.color24to16(0x383838));  // eye,  #383838
  palette->set(DrawingLocation::kSkin,
               M5.Lcd.color24to16(0xfac2a8));  // skin, #fac2a8
  palette->set(DrawingLocation::kCheek1,
               M5.Lcd.color24to16(0xea9c60));  // cheek, #ea9c60
}

void assignLeonaPalette(ColorPalette* palette) {
  palette->set(DrawingLocation::kIris1,
               M5.Lcd.color24to16(0x63434F));  // iris 1, #63434F
  palette->set(DrawingLocation::kEyelash, M5.Lcd.color24to16(0x63434F));
  palette->set(DrawingLocation::kEyelid,
               M5.Lcd.color24to16(0x63436F));  // #63436F
  palette->set(DrawingLocation::kIrisBackground, TFT_BLACK);
  palette->set(DrawingLocation::kIris2,
               M5.Lcd.color24to16(0xFBA148));  // iris2, #FBA148
  palette->set(DrawingLocation::kPupil,
               M5.Lcd.color24to16(0x2D493A));  // #2D493A
  palette->set(DrawingLocation::kEyeHighlight, TFT_WHITE);
  palette->set(DrawingLocation::kSkin,
               M5.Lcd.color24to16(0xfac2a8));  // skin
  palette->set(DrawingLocation::kEyeBrow, M5.Lcd.color24to16(0x321F39));
  palette->set(DrawingLocation::kMouthBackground, TFT_BLACK);
  palette->set(DrawingLocation::kCheek1,
               TFT_PINK);  // cheek
}
}  // namespace m5avatar
#endif  // PALETTE_EXAMPLES_HPP