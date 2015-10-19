#pragma GCC optimize ("-O2")

// 1000 iterations of NEXT(&blah) takes 12.7ms.
// 12,7us per iteration => 78740 iterations/sec.
// 78740 iterations/sec / 24 frames/sec = 3280 iterations/frame.
// That seems pretty good since we only need about 200-300 iterations/frame,
// not counting that we really need quadrature oscillators, and two of them,
// and whatever other fancy stuff on top of that.

#ifdef ARDUINO
  #include <stdint.h>
#else
  #include <stdio.h>
  #define int16_t   short
  #define uint16_t  unsigned short
  #define int32_t   long
  #define uint32_t  unsigned long
#endif

#ifdef ARDUINO
  #include "mult16x16.h"
  #define times_signed(a, b) ({int16_t _r; FMultiS16X16to16(_r, (a), (b)); _r;})
  #define times_unsigned(a, b) ({int16_t _r; FMultiSU16X16to16(_r, (a), (b)); _r;})
#else
  #define times_signed(a, b) ((int16_t)(((int32_t)(a) * (int32_t)(b)) >> 15))
  #define times_unsigned(a, b) ((int16_t)(((int32_t)(a) * (uint32_t)(uint16_t)(b)) >> 15))
#endif

#define fix(n) ((int16_t)(int32_t)((double)(n) * (uint16_t)(1 << 15)))

static __inline__ int16_t zsin(int16_t z);
#define zcos(z) (zsin(z - fix(0.5)))

#define ISA(SUPER) struct SUPER super_type
#define SELF(TYPE) struct TYPE *self= (struct TYPE *) in

#define NEXT(pd1) ((pd1)->next(pd1))

struct d1 {
  int16_t (*next)(struct d1 *p);
};

#define NEW_D1(NEXT) .super_type = {.next = &(NEXT)}

struct constant {
  ISA(d1);
  int16_t value;
};

#define NEW_CONSTANT(VALUE) { NEW_D1(constant_next), .value = VALUE }

int16_t
constant_next(struct d1 *in) {
  SELF(constant);
  return self->value;
}

struct ramp {
  ISA(d1);
  int16_t accum;
  struct d1 *delta;
};

#define NEW_RAMP(INIT, DELTA) { \
    NEW_D1(ramp_next),				\
    .accum = INIT,				\
    .delta = (struct d1 *)&(DELTA),		\
  }

int16_t
ramp_next(struct d1 *in) {
  SELF(ramp);
  return self->accum += NEXT(self->delta);
}

struct xform {
  ISA(d1);
  struct d1 *child;
  int16_t (*xform)(int16_t);
};

int16_t
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

int16_t
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

int16_t
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
  int16_t phase = NEXT(self->p);
*/

struct d2 {
  void (*next)(struct d2 *p);
  int16_t x;
  int16_t y;
};

#define X(o) (o->super_type.x)
#define Y(o) (o->super_type.y)

struct quadrature {
  ISA(d2);
  struct d1 *accum;
  struct d1 *phase;
  int16_t (*zx)(int16_t in);
  int16_t (*zy)(int16_t in);
};

void
quadrature_next(struct d2 *in) {
  SELF(quadrature);
  int16_t accum = NEXT(self->accum);
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
  int16_t angle = NEXT(self->angle);
  // XXX s and c can overflow here.
  int16_t s = zsin(angle) * 2;
  int16_t c = zcos(angle) * 2;
  X(self) = self->child->x * c - self->child->y * s;
  Y(self) = self->child->x * s + self->child->y * c;
}

// Compute sine(z)/2, where z goes from -1 to 1 representing angles of
// -pi to pi.
//
static __inline__ int16_t
zsin(int16_t z)
{
  // Reflect z back into [-1/2, 1/2].

  if ((int16_t)(z + fix(0.5)) < 0) {
    z = fix(1) - z;
  }

  // http://www.coranac.com/2009/07/sines/:
  // The third-order approximation for sin(x), optimized for sin(pi/2) = 1
  // and slope at pi/2 = 0 (i.e., local maximum) is
  // sin(x) = 3/pi*x - 4/pi^3*x^3
  // This is a crude approximation but it probably good enbough.
  // Rather than using x in radians, my phase accumulator z runs from -1 to 1
  // representing angles x of -pi to pi, so z = x/pi or x = z*pi.
  // Substituting x = z*pi:
  // sin(z[*pi]) = 3/pi*z*pi - 4/pi^3*z^3*pi^3 
  //   = 3*z - 4*z^3
  //   = z*(3 - 4*z^2)
  // But the range is outside the range of signed 1.15 numbers, and we
  // need to scale the sine because we want it in the range -1/2 to 1/2
  // for subsequent calculations, so calculate sin(z)/2:
  // sin(z)/2 = z*(3/2 - 2*z^2)
  // 3/2 can be represented as unsigned 1.15, and 3/2 - 2*z^2 also fits
  // in unsigned 1.15, so the final multiply will be fsigned * funsigned.

  return times_unsigned(z, (fix(1.5) - (times_signed(z, z) << 1)));
}

#ifndef ARDUINO
// For debugging.
double unfix(int16_t n) {
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
