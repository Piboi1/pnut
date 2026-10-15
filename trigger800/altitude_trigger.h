// Pure logic for the 800 ft trigger. No Arduino dependencies, so it can be
// unit tested on a PC (see test/test_trigger.cpp).
#pragma once
#include <math.h>

namespace alt {

constexpr float FT_PER_M = 1.0f / 0.3048f;  // 1 ft = 0.3048 m exactly

// Barometric altitude above the pad (feet), given current pressure and the
// pressure measured at the pad. Pressures in the same units (Pa or hPa).
inline float altitudeFt(float pressure, float padPressure) {
  float meters = 44330.77f * (1.0f - powf(pressure / padPressure, 0.190263f));
  return meters * FT_PER_M;
}

// Fires when altitude reaches thresholdFt. Stays fired until altitude falls
// below thresholdFt - hysteresisFt, so sensor noise near 800 ft can't make the
// output chatter.
class Trigger {
 public:
  Trigger(float thresholdFt, float hysteresisFt)
      : threshold_(thresholdFt), hysteresis_(hysteresisFt) {}

  bool update(float altFt) {
    if (!fired_ && altFt >= threshold_) fired_ = true;
    else if (fired_ && altFt < threshold_ - hysteresis_) fired_ = false;
    return fired_;
  }
  bool fired() const { return fired_; }

 private:
  float threshold_, hysteresis_;
  bool fired_ = false;
};

}  // namespace alt
