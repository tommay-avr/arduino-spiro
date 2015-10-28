/*
  Pin usage:

  PC0 - PC5: analog in
  PD6 - PD7 / PCINT22 - PCINT23: encoder in
  PB2: nSS out (DAC chip select)
  PB1: nLDAC out (DAC latch load)

  PB2 - PB5: SPI
  PB6 - PB7: xtal
  PC6: nRESET
*/

#pragma GCC optimize ("-O2")

#include <stdint.h>
#include "fixed_point.h"
#include "zsin.h"

struct point {
  fix16_t x;
  fix16_t y;
};

#define ADC_CHANNELS 6
static int adc_values[ADC_CHANNELS];

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

#include <avr/io.h>
#include <avr/cpufunc.h>	// _NOP
#include "dac.h"

static struct {
  uint8_t last_a;
  uint8_t last_b;
  uint8_t stable_a;
  uint8_t stable_b;
} encoder;

#define ENCODER_A() ((PIND & _BV(PD6)) != 0)
#define ENCODER_B() ((PIND & _BV(PD7)) != 0)

static void initialize(void);
static void run(void);

static unsigned long then;

void
setup() {
  Serial.begin(9600);
  initialize();
  run();
}

void
loop() {}

static void
initialize() {
  // Encoder.  Pins PD6 and PD7 are input (default) with pull-ups enabled.

  PORTD = _BV(PORTD6) | _BV(PORTD7);

  // Enable their pin changfe interrupts.

  PCMSK2 |= _BV(PCINT22) | _BV(PCINT23);
  PCICR |= _BV(PCIE2);

  // Set the current encoder pin states.

  encoder.last_a = encoder.stable_a = ENCODER_A();
  encoder.last_b = encoder.stable_b = ENCODER_B();

  // Knobs (PC0-5/ADC0-5) are input (default) with pull-ups disabled
  // (default) and digital input buffers disabled.

  DIDR0 |= _BV(ADC0D)|_BV(ADC1D)|_BV(ADC2D)|_BV(ADC3D)|_BV(ADC4D)|_BV(ADC5D);

  // ADC setup:

  // External AVcc reference.

  ADMUX |= _BV(REFS0);

  // Left adjust ADC result in ADCH:ADCL.  It's just easier to deal
  // with that way.

  ADMUX |= _BV(ADLAR);

  // ADC clock prescaler is /128.  ADC frequency is 16MHz / 128 =
  // 125kHz (50-200kHz).
  
  ADCSRA = 7;

  // Enable the ADC.

  ADCSRA |= _BV(ADEN);

  // nSS/PB2 is the DAC chip select.  Output high.

  DDRB |= _BV(DDB2);		// PB2 is output.
  PORTB |= _BV(PORTB2);		// PB2 is high.

  // PB1 is nLDAC, the DAC latch.  Output high.

  DDRB |= _BV(DDB1);		// PB1 is output.
  PORTB |= _BV(PORTB1);		// PB1 is high.

  // Enable pull-ups on unused/floating input pins.

  // XXX PORTB |= _BV(PB1) | _BV(PB2) | _BV(PB5);

  // SPI.  Enable as master, set mode, clk = fOsc/2 (max).  Enable
  // master after setting nSS/PB2 to output otherwise if nSS is input
  // low the chip will switch to slave mode.

  SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPHA) | _BV(CPOL);
  SPSR = _BV(SPI2X);

  // PB5/SCK and PB3/MOSI are output.

  DDRB |= _BV(DDB5) | _BV(DDB3);

  // Timer 2.  This is used to regulate when the DAC is updated.  It
  // runs in CTC and we wait until it reaches OCR2A and is reset
  // before updating the DAC.

  TCCR2A = _BV(WGM21);		// CTC mode, pin not used.
  TCCR2B = 3;			// Prescale /32, clk = 500kHz.
  OCR2A = 103;			// Ticks @ 50kHz/104 = 4.8kHz.

  // Make sure interrupts are enabled.

  sei();
}

static void
run() {
  int adc_channel = 0;

  int count = 0;

  for (;;) {
    // Calculate the next point.

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

    // Wait for the timer tick.

    loop_until_bit_is_set(TIFR2, OCF2A);

    // Clear the tick.

    TIFR2 |= _BV(OCF2A);

    // Write the point to the DAC.  Convert from signed two's
    // complement to unsigned.

    write_dac(p.x + 0x8000, p.y + 0x8000);

    // If the ADC is done then read the value, advance the channel,
    // and start the next conversion.

    if ((ADCSRA & _BV(ADSC)) == 0) {
      *((uint8_t *)(&adc_values[adc_channel]) + 0) = ADCL;
      *((uint8_t *)(&adc_values[adc_channel]) + 1) = ADCH;

      if (++adc_channel == ADC_CHANNELS) {
	adc_channel = 0;
      }

      ADMUX = _BV(REFS1) | _BV(ADLAR) | adc_channel;
      ADCSRA |= _BV(ADSC);
    }
  }
}

#define NUM_PROGRAMS (1)	// XXX

// program is updated from an interrupt handler so it's volatile.  But
// it doesn't need atomic access since it's a just a byte.

volatile static uint8_t program;

static __inline__ void ccw(void) {
  uint8_t new_program = program - 1;
  if (new_program == 0xFF) {
    new_program = NUM_PROGRAMS - 1;
  }
  program = new_program;
}

static __inline__ void cw(void) {
  uint8_t new_program = program + 1;
  if (new_program == NUM_PROGRAMS) {
    new_program = 0;
  }
  program = new_program;
}

// Handle encoder pin change interrupts.  This is intended to be
// bounce-free, but may have issues depending on how the chip handles
// interrrupts on signals that switch, then switch back quickly.

ISR (PCINT2_vect) {
  // Which pin changed?  Only one pin should change, the other should
  // be stable.  So whichever pin changed, we update the stable state
  // of the other pin.  But if the interrupting pin is unstable it
  // might have changed back by the time we read it.  Then will it
  // interrupt again so we can read the changed value?

  uint8_t a = ENCODER_A();
  uint8_t b = ENCODER_B();

  if (encoder.last_a != a) {
    encoder.last_a = a;
    // a changed, so b is stable at its current value.  But only
    // update b and fire ccw if this is the first time we've read this
    // stable value.  I.e., a may be bouncing up and down, but we only
    // want to update b once, and skip this stuff if we've already
    // updated it.
    if (encoder.stable_b != b) {
      encoder.stable_b = b;
      // Fire ccw if we've come back around to 00.
      if (b == 0 && encoder.stable_a == 0) {
	ccw();
      }
    }
  }

  // Same logic, but b changed and a is stable.

  if (encoder.last_b != b) {
    encoder.last_b = b;
    if (encoder.stable_a != a) {
      encoder.stable_a = a;
      if (a == 0 && encoder.stable_b == 0) {
	cw();
      }
    }
  }
}

#else

#include <stdio.h>

int
main(int argc, char **argv) {
  for (;;) {
    struct point p;
    circles_main(&p);
    printf("%d, %d\n", p.x, p.y);
  }
}

#endif
