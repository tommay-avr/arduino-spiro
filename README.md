# arduino-spiro

An Arduino sketch to create spirograph designs on a scope on XY mode,
or any other XY device that can be driven from analog voltages.  I'm thinking
some kind of laser device.

Requires outboard DACs.  I'm using an MCP4812.

It's also intended to be used with a bunch of pots connected to the
ADC inputs, which can control things as specified in a *.spiro file.

## Things of note:

- I use bare metal AVR code instead of the Arduino library/header stuff.
The Arduino stuff, especially the Serial class, is useful for development,
but I like to know things are being done right under the hood so I've
just removed the hood.
- The algorithms to generate the designs are specfied in *.spiro files
which hook up ramp generators, knobs, quadratures, and other components
using a simple ruby data structure which then creates C code to do
the algorithm.
- It's possible I could just do things in C++ instead of using *.spiro
and ruby, but C++ is too awful to even consider.  And I can do some
simple transformations at code generation time to make things more
efficient.
- I use multiplication routines adapted from https://github.com/rekka/avrmultiplication.  I've added support for fractional 1.15 numbers.  My fork is at
https://github.com/tommay/avrmultiplication.

## TODO

- Use a rotary encoder to select from multiple design programs.
