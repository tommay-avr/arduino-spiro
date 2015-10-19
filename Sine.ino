#pragma GCC optimize ("-O2")

// 1000 iterations of NEXT(&blah) takes 12.7ms.
// 12,7us per iteration => 78740 iterations/sec.
// 78740 iterations/sec / 24 frames/sec = 3280 iterations/frame.
// That seems pretty good since we only need about 200-300 iterations/frame,
// not counting that we really need quadrature oscillators, and two of them,
// and whatever other fancy stuff on top of that.

#include <stdint.h>
#include "fixed_point.h"
#include "zsin.h"

#ifndef ARDUINO
  #include <stdio.h>
#endif

#define ISA(SUPER) struct SUPER super_type
#define SELF(TYPE) struct TYPE *self= (struct TYPE *) in

#define NEXT(pd1) ((pd1)->next(pd1))

struct d1 {
  fix16_t (*next)(struct d1 *p);
};

#define NEW_D1(NEXT) .super_type = {.next = &(NEXT)}

struct constant {
  ISA(d1);
  fix16_t value;
};

#define NEW_CONSTANT(VALUE) { NEW_D1(constant_next), .value = VALUE }

fix16_t
constant_next(struct d1 *in) {
  SELF(constant);
  return self->value;
}

struct ramp {
  ISA(d1);
  fix16_t accum;
  struct d1 *delta;
};

#define NEW_RAMP(INIT, DELTA) { \
    NEW_D1(ramp_next),				\
    .accum = INIT,				\
    .delta = (struct d1 *)&(DELTA),		\
  }

fix16_t
ramp_next(struct d1 *in) {
  SELF(ramp);
  return self->accum += NEXT(self->delta);
}

#if 0
struct up_down {
  ISA(d1);
  fix16_t accum;
  struct d1 *delta;
};

#define NEW_UP_DOWN(INIT, DELTA) { \
    NEW_D1(up_down_next),			\
    .accum = INIT,				\
    .delta = (struct d1 *)&(DELTA),		\
  }

// As accum varies between -1 and 1, it is reflected back to stay
// within -0.5 to 0.5.
//
fix16_t
up_down_next(struct d1 *in) {
  SELF(up_down);
  fix16_t accum = self->accum += NEXT(self->delta);

#if 0
  // Compiler converts this to z < fix(0.5), buggy.
  if ((fix16_t)(accum + fix(0.5)) < 0) {
#else
  if (accum < fix(-0.5) || accum > fix(0.5)) {
#endif
    return fix(1) - accum;
  }
  else {
    return accum;
  }
}
#endif

struct scale {
  ISA(d1);
  struct d1 *child;
  struct d1 *scale;
};

#define NEW_SCALE(INIT, CHILD, SCALE) {		\
    NEW_D1(scale_next),				\
    .child = (struct d1 *)&(SCALE),		\
    .scale = (struct d1 *)&(SCALE),		\
  }

fix16_t
scale_next(struct d1 *in) {
  SELF(scale);
  return times_signed(NEXT(self->child), NEXT(self->scale));
}

struct xform {
  ISA(d1);
  struct d1 *child;
  fix16_t (*xform)(fix16_t);
};

fix16_t
xform_next(struct d1 *in) {
  SELF(xform);
  return self->xform(NEXT(self->child));
}

// XXX ISA(xform)?  But here zsin is hardcoded.
struct to_zsin {
  ISA(d1);
  struct d1 *angle;
};

#define NEW_TO_ZSIN(ANGLE) { \
    NEW_D1(to_zsin_next),			\
    .angle = (struct d1 *)&(ANGLE),		\
  }

fix16_t
to_zsin_next(struct d1 *in) {
  SELF(to_zsin);
  return zsin(NEXT(self->angle));
}

struct plus {
  ISA(d1);
  struct d1 *p1;
  struct d1 *p2;
};

#define NEW_PLUS(P1, P2) { \
    NEW_D1(plus_next), \
    .p1 = (struct d1 *)&(P1),			\
    .p2 = (struct d1 *)&(P2),			\
  }

fix16_t
plus_next(struct d1 *in) {
  SELF(plus);
  return NEXT(self->p1) + NEXT(self->p2);
}

struct d2 {
  void (*next)(struct d2 *p);
  fix16_t x;
  fix16_t y;
};

#define X(o) ((o)->super_type.x)
#define Y(o) ((o)->super_type.y)

struct quadrature {
  ISA(d2);
  struct d1 *accum;
  struct d1 *phase;
  fix16_t (*zx)(fix16_t in);
  fix16_t (*zy)(fix16_t in);
};

#define NEW_QUADRATURE(ACCUM, PHASE, ZX, ZY) {	\
    NEW_D1(quadrature_next),			\
    .accum = (struct d1 *)&(ACCUM),		\
    .phase = (struct d1 *)&(PHASE),		\
    .zx = ZX,					\
    .zy = ZY,					\
  }

void
quadrature_next(struct d2 *in) {
  SELF(quadrature);
  fix16_t accum = NEXT(self->accum);
  X(self) = self->zx(accum);
  Y(self) = self->zy(accum + NEXT(self->phase));
}

struct rotate {
  ISA(d2);
  struct d2 *child;
  struct d1 *angle;
};

void
rotate_next(struct d2 *in) {
  SELF(rotate);
  NEXT(self->child);
  fix16_t angle = NEXT(self->angle);
  // XXX s and c can overflow here.
  fix16_t s = zsin(angle) * 2;
  fix16_t c = zcos(angle) * 2;
  X(self) = self->child->x * c - self->child->y * s;
  Y(self) = self->child->x * s + self->child->y * c;
}

#ifndef ARDUINO
// For debugging.
double unfix(fix16_t n) {
  return (double)n / (double)(uint16_t)(1 << 15);
}
#endif

// Creates a square, with a corresponding quadrature signal.
// 0 -> 0.5 ramps from -0.5 to 0.5
// 0.5 -> +/-1 stays at 0.5
// -1 -> -0.5 ramps from 0.5 to -0.5
// -0.5 -> 0 stays at -0.5
//
fix16_t
square(fix16_t n) {
  if (n >= 0) {
    if (n < fix(0.5)) {
      return fix(-0.5) + n*2;
    }
    else {
      return fix(0.5);
    }
  }
  else {
    if (n < fix(-0.5)) {
      return fix(-1.5) - n*2;
    }
    else {
      return fix(-0.5);
    }
  }
}

// As n varies between -1 and 1, it is reflected back to stay within
// -0.5 to 0.5.  Creates a diamond, with a corresponding quadrature
// signal.
//
fix16_t
diamond(fix16_t n) {
#if 0
  // Compiler converts this to z < fix(0.5), buggy.
  if ((fix16_t)(n + fix(0.5)) < 0) {
#else
  if (n < fix(-0.5) || n > fix(0.5)) {
#endif
    return fix(1) - n;
  }
  else {
    return n;
  }
}

// circle_quad is a circle with varying quadrature phase.
struct constant accum_delta = NEW_CONSTANT(327);
struct ramp accum_ramp = NEW_RAMP(0, accum_delta);
// This gives a varying phase.
struct constant phase_delta = NEW_CONSTANT(0);
struct ramp phase_ramp = NEW_RAMP(0, phase_delta);
// These could both be zsin:
struct quadrature circle_quad = NEW_QUADRATURE(accum_ramp, phase_ramp, zcos, zsin);

// square_quad is a square with varying quadrature phase.
struct quadrature square_quad = NEW_QUADRATURE(accum_ramp, phase_ramp, square, square);

// diamond_quad is a diamond with varying quadrature phase.
struct quadrature diamond_quad = NEW_QUADRATURE(accum_ramp, phase_ramp, diamond, diamond);

//#define RUN_ME circle_quad
//#define RUN_ME square_quad
#define RUN_ME diamond_quad

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
    NEXT((struct d1 *)&RUN_ME);
#if 1
    if (++count == 1000) {
      count = 0;
      unsigned long now = micros();
      unsigned long delta = now - then;
      then = now;
      Serial.println(delta);
    }
#else
    Serial.print(X(&RUN_ME));
    Serial.print(", ");
    Serial.println(Y(&RUN_ME));
#endif
  }
}

#else

int
main(int argc, char **argv) {
  for (;;) {
    printf("%d\n", NEXT((struct d1 *)&blah));
  }
}

#endif
