#pragma GCC optimize ("-O2")

#include <stdint.h>
#include "fixed_point.h"
#include "zsin.h"

#ifndef ARDUINO
#include <stdio.h>
#endif

struct point {
  fix16_t x;
  fix16_t y;
};

// As n varies between -1 and 1, it is reflected back to stay within
// -0.5 to 0.5.  Creates a diamond, with a corresponding quadrature
// signal.
//
static __inline__ fix16_t
diamond(fix16_t n) {
  // Need to use an inner cast to unsigned because signed integer overflow
  // is undefined and this breaks with -O2.
  if ((fix16_t)((ufix16_t)n + fix(0.5)) < 0) {
    return fix(1) - n;
  }
  else {
    return n;
  }
}

#include "spiro.h"

#ifdef ARDUINO

static unsigned long then;

void
setup() {
  Serial.begin(9600);
}

void
loop() {
  int count = 0;
  for (;;) {
    struct point p;
    spiro(&p);
#if 1
    if (++count == 1000) {
      count = 0;
      unsigned long now = micros();
      unsigned long delta = now - then;
      then = now;
      Serial.println(delta);
    }
#else
    Serial.print(p.x);
    Serial.print(", ");
    Serial.println(p.y);
#endif
  }
}

#else

int
main(int argc, char **argv) {
  for (;;) {
    struct point p;
    spiro(&p);
    printf("%d, %d\n", p.x, p.y);
  }
}

#endif
