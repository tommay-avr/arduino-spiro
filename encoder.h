#ifndef _encoder_h_
#define _encoder_h_

#include <stdint.h>
#include "inline.h"

// last_? is the most recent value read from each pin.  stable_? is
// the value read when the other pin most recently changed state.

struct encoder {
  uint8_t last_a;
  uint8_t last_b;
  uint8_t stable_a;
  uint8_t stable_b;
};

#define read_encoder(PORT, PIN) (((PORT) & _BV(PIN)) != 0)

INLINE(
void,
init_encoder,
(struct encoder *p, uint8_t (*encoder_a)(void), uint8_t (*encoder_b)(void)))
{
  p->last_a = p->stable_a = encoder_a();
  p->last_b = p->stable_b = encoder_b();
}

// handle_encoder() is called from the encoder ISR when either encoder
// pin changes state.  It calls cw() or ccw() once per quadrature
// cycle if a movement has been detected.  This is intended to be
// bounce-free, but may have issues depending on how the chip handles
// interrrupts on signals that switch, then switch back quickly.  This
// is explained in the handle_encoder() comment.

INLINE(
void,
handle_encoder,
(struct encoder *p, uint8_t (*encoder_a)(void), uint8_t (*encoder_b)(void),
 void (*cw)(void), void (*ccw)(void)))
{
  // Which pin changed?  Only one pin should change, the other should
  // be stable.  So whichever pin changed, we update the stable state
  // of the other pin.  But if the interrupting pin is unstable it
  // might have changed back by the time we read it.  Then will it
  // interrupt again so we can read the changed value?

  uint8_t a = encoder_a();
  uint8_t b = encoder_b();

  if (p->last_a != a) {
    p->last_a = a;
    // a changed, so b is stable at its current value.  But only
    // update b and fire ccw if this is the first time we've read this
    // stable value.  I.e., a may be bouncing up and down, but we only
    // want to update b once, and skip this stuff if we've already
    // updated it.
    if (p->stable_b != b) {
      p->stable_b = b;
      // Fire ccw if we've come back around to 00.
      if (b == 0 && p->stable_a == 0) {
	ccw();
      }
    }
  }

  // Same logic, but b changed and a is stable.

  if (p->last_b != b) {
    p->last_b = b;
    if (p->stable_a != a) {
      p->stable_a = a;
      if (a == 0 && p->stable_b == 0) {
	cw();
      }
    }
  }
}

#endif /* ndef _encoder_h_ */
