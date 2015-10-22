/*
  Pin usage:

  PC0 - PC5: analog in
  PD6 - PD7: encoder in
  PB2: nSS out (DAC chip select)
  PB1: nLDAC out (DAC latch load)

  PB2 - PB5: SPI
  PB6 - PB7: xtal
  PC6: nRESET
*/

#pragma GCC optimize ("-O2")

#include <avr/io.h>
#include <avr/cpufunc.h>	// _NOP
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

  // SPI.  Enable as master, set mode, clk = fOsc/2 (max).

  SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPHA) | _BV(CPOL);
  SPSR = _BV(SPI2X);

  // Encoder.  Pins PD6 and PD7 are input (default) with pull-ups enabled.

  PORTD = _BV(PORTD6) | _BV(PORTD7);

  // Knobs (PC0-5/ADC0-5) are input (default) with pull-ups disabled
  // (default) and digital input buffers disabled.

  DIDR0 |= _BV(ADC0D)|_BV(ADC1D)|_BV(ADC2D)|_BV(ADC3D)|_BV(ADC4D)|_BV(ADC5D);

  // PB2/nSS is the DAC chip select.  Output high.

  DDRB |= _BV(DDB2);		// PB2 is output.
  PORTB |= _BV(PORTB2);		// PB2 is high.

  // PB1 is nLDAC, the DAC latch.  Output high.

  DDRB |= _BV(DDB1);		// PB1 is output.
  PORTB |= _BV(PORTB1);		// PB1 is high.

  // Enable pull-ups on unused/floating input pins.

  // XXX PORTB |= _BV(PB1) | _BV(PB2) | _BV(PB5);
}

void
loop() {
  int count = 0;
  for (;;) {
    struct point p;
    circles_main(&p);
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
