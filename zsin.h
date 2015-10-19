#ifndef _zsin_h_
#define _zsin_h_

#include "fixed_point.h"

// Compute sine(z)/2, where z goes from -1 to 1 representing angles of
// -pi to pi.
//
static __inline__ fix16_t
zsin(fix16_t z)
{
  // Reflect z back into [-1/2, 1/2].

  if ((fix16_t)(z + fix(0.5)) < 0) {
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

static __inline__ fix16_t
zcos(fix16_t z)
{
  return zsin(z + fix(0.5));
}

#endif /* ndef _zsin_h_ */
