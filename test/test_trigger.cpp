#include <cassert>
#include <cmath>
#include <cstdio>
#include "../trigger800/altitude_trigger.h"

int main() {
  const float pad = 101325.0f;

  // 0 ft at the pad.
  assert(std::fabs(alt::altitudeFt(pad, pad)) < 0.01f);

  // Standard atmosphere: ~98,430 Pa is ~800 ft above a 101,325 Pa pad.
  float a = alt::altitudeFt(98430.0f, pad);
  printf("98430 Pa -> %.1f ft\n", a);
  assert(std::fabs(a - 800.0f) < 5.0f);

  // Round trip: pressure drop for exactly 800 ft = 243.84 m.
  float p800 = pad * powf(1.0f - 243.84f / 44330.77f, 1.0f / 0.190263f);
  printf("800 ft -> %.0f Pa (%.1f hPa below pad)\n", p800, (pad - p800) / 100.0f);
  assert(std::fabs(alt::altitudeFt(p800, pad) - 800.0f) < 0.5f);

  // Trigger: fires at 800, holds through noise, releases below 790.
  alt::Trigger t(800.0f, 10.0f);
  assert(!t.update(799.9f));
  assert(t.update(800.0f));
  assert(t.update(795.0f));   // still held (inside hysteresis)
  assert(!t.update(789.9f));  // released
  assert(t.update(801.0f));   // re-arms

  puts("all tests passed");
  return 0;
}
