#include "inline.h"

// Write to the MCP8412 DAC via SPI.

// Constants for DAC commands.

enum {
  DAC_CHANNEL_A = 0x0000,
  DAC_CHANNEL_B = 0x8000,
  DAC_GAIN_1X = 0x2000,
  DAC_GAIN_2X = 0x0000,
  DAC_ACTIVE = 0x1000,
  DAC_SHDN = 0x0000,
};

// Write a byte via SPI.  XXX This busyloops until the write is
// complete.

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

// Writes the 12 high bits of value to either DAC_CHANNEL_A or
// DAC_CHANNEL_B.

INLINE(
void,
write_dac_channel,
(uint16_t channel, uint16_t val))
{
  // Assert DAC chip select.  The chip select needs to be high at
  // least 15nS between DAC writes.  No problem
  PORTB &= ~_BV(PORTB2);
  uint16_t data = channel | DAC_GAIN_2X | DAC_ACTIVE | (val >> 4);
  write_spi_byte(data >> 8);
  write_spi_byte(data);
  // De-assert DAC chip select.
  PORTB |= _BV(PORTB2);
}

// Write to both channels of the DAC.  LDAC_IS_GROUNDED is not
// defined, so we toggle LDAC here which makes the DAC latch both
// values simultaneously.

INLINE(
void,
write_dac,
(uint16_t a, uint16_t b))
{
  write_dac_channel(DAC_CHANNEL_A, a);
  write_dac_channel(DAC_CHANNEL_B, b);
#ifndef LDAC_IS_GROUNDED
  // Pulse nLDAC low to latch both data channels.
  // Need tLS >= 40nS between nCS = 1 and nLDAC = 0.  Should be ok.
  // Yes, we've got 108nS.
  PORTB &= ~_BV(PORTB1);
  // Need nLDAC pulse width tLD >= 100nS.  I.e., a couple insn clocks.
  // This gets 250nS.
  _NOP(); _NOP();
  PORTB |= _BV(PORTB1);
#endif
}
