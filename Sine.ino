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

/*
-0.25 -> 0.25 ramps from -0.5 to 0.5
0.25 -> 0.75 stays at 0.5
0.75 -> +/- 1 -> -0.75  ramps from 0.5 to -0.5
-0.75 -> -0.25 stays at -0.5

DECL_NEXT(square) {
  fix16_t phase = NEXT(self->p);
*/

struct d2 {
  void (*next)(struct d2 *p);
  fix16_t x;
  fix16_t y;
};

#define X(o) (o->super_type.x)
#define Y(o) (o->super_type.y)

struct quadrature {
  ISA(d2);
  struct d1 *accum;
  struct d1 *phase;
  fix16_t (*zx)(fix16_t in);
  fix16_t (*zy)(fix16_t in);
};

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

struct constant half = NEW_CONSTANT(fix(0.5));
struct constant delta = NEW_CONSTANT(327);
struct ramp ramp1 = NEW_RAMP(0, delta);
struct plus plus1 = NEW_PLUS(half, ramp1);
struct to_zsin blah = NEW_TO_ZSIN(plus1);

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
    NEXT((struct d1 *)&blah);
    if (++count == 1000) {
      count = 0;
      unsigned long now = micros();
      unsigned long delta = now - then;
      then = now;
#if 0
      Serial.print("micros: ");
#endif
      Serial.println(delta);
    }
  }

  //Serial.println(NEXT((struct d1 *)&blah));
}

#else

int
main(int argc, char **argv) {
  for (;;) {
    printf("%d\n", NEXT((struct d1 *)&blah));
  }
}

#endif
