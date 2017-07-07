/*
  Pin usage:

  PC0 - PC5: analog in [analog 0 - 5]
  PD6 - PD7 / PCINT22 - PCINT23: encoder in [pin 6/7]
  PB1: nLDAC out (DAC latch load) [pin 9]
  PB2: DAC chip select [pin 10]
    This is SPI bus stuff managed by the Atmega328.  We're doing SPI
    as a master so PB2/nSS is just a convenient output pin connected
    to the nSS input of the DAC:
  PB3: MOSI [pin 11]
  PB5: SCK [pin 13]
*/

// This can be compiled with ARDUINO defined to generate code an
// Arduino, or without ARDUINO defined to generate code that will run
// on a regular system and print the (x,y) values that would be sent
// to the DAC.

#pragma GCC optimize ("-O2")

#include <stdint.h>
#include "encoder.h"
#include "inline.h"
#include "fixed_point.h"
#include "zsin.h"

struct point {
  fix16_t x;
  fix16_t y;
};

// encoder holds the state of the rotary encoder pins.  It's updated
// in an ISR.  When the encoder moves the ISR fires and may call cw()
// or ccw() to update the program state (encoder_value).  See the
// calls to init_encoder() and handle_encoder() below.  init_encoder()
// could store the read_encoder parameters and the cw/ccw pointers,
// but for now I've chosen to use explicit inline functions and macros
// instead of doing indirect calls via struct encoder.

static struct encoder encoder;

INLINE(uint8_t, encoder_a, (void)) { return read_encoder(PIND, PD6); }
INLINE(uint8_t, encoder_b, (void)) { return read_encoder(PIND, PD7); }

// encoder_value is updated from an ISR so it's volatile.  But it
// doesn't need atomic access since it's just a byte.

volatile static uint8_t encoder_value;

// One adc channel is read and its adc_value updated each time through
// the main loop.  Assuming the adc cycle is complete.

#define ADC_CHANNELS 6
static uint16_t adc_values[ADC_CHANNELS];

// As n varies between -1 and 1, it is reflected back to stay within
// -0.5 to 0.5.  Creates a diamond, when used with a corresponding
// quadrature signal.

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

#include "spiro.h"		// Get programs[].

// programs comes from spiro.h which is created by spiro.rb from the
// *.spiro files.  See Makefile.

#define NUM_PROGRAMS (sizeof(programs) / sizeof(programs[0]))

#ifdef ARDUINO

#include <avr/io.h>
#include <avr/cpufunc.h>	// _NOP
#include "dac.h"

static void initialize(void);
static void run(void);

// setup() and loop() are called by the Arduino framework.  We do the looping
// in our run() function so loop() is a noop.

void
setup() {
  Serial.begin(9600); // Initialize the Serial over USB for debug logging.
  initialize();
  run();
}

void
loop() {}

static void
initialize() {
  // Encoder.  Pins PD6 and PD7 are input (default) with pull-ups enabled.

  PORTD = _BV(PORTD6) | _BV(PORTD7);

  // Enable their pin change interrupts.

  PCMSK2 |= _BV(PCINT22) | _BV(PCINT23);
  PCICR |= _BV(PCIE2);

  // Fetch the current encoder pin states.

  init_encoder(&encoder, encoder_a, encoder_b);

  // Knobs (PC0-5/ADC0-5) are input (default) with pull-ups disabled
  // (default) and digital input buffers disabled.

  DIDR0 |= _BV(ADC0D)|_BV(ADC1D)|_BV(ADC2D)|_BV(ADC3D)|_BV(ADC4D)|_BV(ADC5D);

  // ADC setup:

  // External AVcc reference.

  ADMUX |= _BV(REFS0);

  // Left adjust ADC result in ADCH:ADCL so we can interpret the result
  // as an unsigned number [0, 65535] or equivalently as a 1.15 number
  // [0, 2).

  ADMUX |= _BV(ADLAR);

  // ADC clock prescaler is /128.  ADC frequency is 16MHz / 128 =
  // 125kHz (50-200kHz).
  
  ADCSRA = 7;

  // Enable the ADC.

  ADCSRA |= _BV(ADEN);

  // PB2 is the DAC chip select.  Output high.

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
  uint8_t adc_channel = 0;

  int count = 0;

  for (;;) {
    // Calculate the next point.

    struct point p;
    programs[encoder_value](&p);

#if 0
    // Output the time taken in every 1000 loops.
    if (++count == 1000) {
      static unsigned long then;
      count = 0;
      unsigned long now = micros();
      unsigned long delta = now - then;
      then = now;
      Serial.println(delta);
    }
#elif 0
    // Output the current point.
    Serial.print(p.x);
    Serial.print(", ");
    Serial.println(p.y);
#endif

    // Wait for the timer tick.

#if 0
    loop_until_bit_is_set(TIFR2, OCF2A);

    // Clear the tick.

    TIFR2 |= _BV(OCF2A);
#endif

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

// Called rom the encoder ISR when the encoder is moved ccw.

static __inline__ void ccw(void) {
  uint8_t new_encoder_value = encoder_value - 1;
  if (new_encoder_value == 0xFF) {
    new_encoder_value = NUM_PROGRAMS - 1;
  }
  encoder_value = new_encoder_value;
}

// Called rom the encoder ISR when the encoder is moved cw.

static __inline__ void cw(void) {
  uint8_t new_encoder_value = encoder_value + 1;
  if (new_encoder_value == NUM_PROGRAMS) {
    new_encoder_value = 0;
  }
  encoder_value = new_encoder_value;
}

// Handle interrupts for both encoder pins.

ISR (PCINT2_vect) {
  handle_encoder(&encoder, encoder_a, encoder_b, cw, ccw);
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
