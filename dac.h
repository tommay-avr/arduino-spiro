#include "inline.h"

enum {
  DAC_CHANNEL_A = 0x0000,
  DAC_CHANNEL_B = 0x8000,
  DAC_GAIN_2X = 0x2000,
  DAC_GAIN_1X = 0x0000,
  DAC_ACTIVE = 0x1000,
  DAC_SHDN = 0x0000,
};

// XXX crappy polling.
INLINE(
void,
write_spi_byte,
(uint8_t data))
{
  SPDR = data;
  // Wait for transmission complete.
  loop_until_bit_is_set(SPSR, SPIF);
  // Read SPDR to clear SPIF.
  SPDR;
}

INLINE(
void,
write_dac_channel,
(uint16_t channel, uint16_t val))
{
  // Assert DAC chip select.
  PORTB &= !_BV(PORTB2);
  uint16_t data = channel | DAC_GAIN_2X | DAC_ACTIVE | (val >> 4);
  write_spi_byte(data >> 8);
  write_spi_byte(data);
  // De-assert DAC chip select.
  PORTB |= _BV(PORTB2);
}

INLINE(
void,
write_dac,
(uint16_t a, uint16_t b))
{
  write_dac_channel(DAC_CHANNEL_A, a);
  write_dac_channel(DAC_CHANNEL_B, b);
#ifndef LDAC_IS_GROUNDED
  // XXX Need tLS >= 40nS between nCS = 1 and nLDAC = 0.  Should be ok.
  PORTB &= ~_BV(PORTB1); // Latch the data.  Or just tie the pin to ground.
  // XXX Need nLDAC pulse width tLD >= 100nS.  I.e., a couple insn clocks.
  _NOP(); _NOP();
  PORTB |= _BV(PORTB1);
#endif
}
