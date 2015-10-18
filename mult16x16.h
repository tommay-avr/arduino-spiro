
/*
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// longRes = intIn1 * intIn2
#define MultiU16X16to32(longRes, intIn1, intIn2) \
asm ( \
"mul %A1, %A2 \n\t" \
"movw %A0, r0 \n\t" \
"mul %B1, %B2 \n\t" \
"movw %C0, r0 \n\t" \
"mul %B2, %A1 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, %3 \n\t" \
"mul %B1, %A2 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, %3 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (longRes) \
: \
"a" (intIn1), \
"a" (intIn2), \
"r" (0) \
) 

// intRes = intIn1 * intIn2 >> 16
#define MultiU16X16toH16(intRes, intIn1, intIn2) \
({ \
char _tmp_; \
asm ( \
"mul %A2, %A3 \n\t" \
"mov %1, r1 \n\t" \
"mul %B2, %B3 \n\t" \
"movw %A0, r0 \n\t" \
"mul %B3, %A2 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"mul %B2, %A3 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (intRes), \
"=&r", (_tmp_) \
: \
"a" (intIn1), \
"a" (intIn2), \
"r" (0) \
) 

// funsigned = funsigned * funsigned
// This is MultiU16X16toH16 shifted left one.
#define FMultiU16X16to16(intRes, intIn1, intIn2) \
({ \
char _tmp_; \
asm ( \
"mul %A2, %A3 \n\t" \
"mov %1, r1 \n\t" \
"mul %B2, %B3 \n\t" \
"movw %A0, r0 \n\t" \
"mul %B3, %A2 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"mul %B2, %A3 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"lsl %1 \n\t" \
"rol %A0 \n\t" \
"rol %B0 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (intRes), \
"=&r", (_tmp_) \
: \
"a" (intIn1), \
"a" (intIn2), \
"r" (0) \
) 

// intRes = intIn1 * intIn2 >> 16 + round
// uses:
// 21 cycles
#define MultiU16X16toH16Round(intRes, intIn1, intIn2) \
({ \
char _tmp_; \
asm ( \
"mul %A2, %A3 \n\t" \
"mov %1, r1 \n\t" \
"mul %B2, %B3 \n\t" \
"movw %A0, r0 \n\t" \
"mul %B3, %A2 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"mul %B2, %A3 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"lsl %1 \n\t" \
"adc %A0, %4 \n\t" \
"adc %B0, %4 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (intRes), \
"=&r" (_tmp_) \
: \
"a" (intIn1), \
"a" (intIn2), \
"r" (0) \
); \
})


// signed16 * signed16
// 22 cycles
#define MultiS16X16to32(longRes, intIn1, intIn2) \
asm ( \
"mul %A1, %A2 \n\t" \
"movw %A0, r0 \n\t" \
"muls %B1, %B2 \n\t" \
"movw %C0, r0 \n\t" \
"mulsu %B2, %A1 \n\t" \
"sbc %D0, %3 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, %3 \n\t" \
"mulsu %B1, %A2 \n\t" \
"sbc %D0, %3 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, %3 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (longRes) \
: \
"a" (intIn1), \
"a" (intIn2), \
"r" (0) \
)


// signed16 * signed 16 >> 16
#define MultiS16X16toH16(intRes, intIn1, intIn2) \
({ \
char _tmp_; \
asm ( \
"mul %A2, %A3 \n\t" \
"mov %1, r1 \n\t" \
"muls %B2, %B3 \n\t" \
"movw %A0, r0 \n\t" \
"mulsu %B3, %A2 \n\t" \
"sbc %B0, %4 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"mulsu %B2, %A3 \n\t" \
"sbc %B0, %4 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (intRes), \
"=&r" (_tmp_) \
: \
"a" (intIn1), \
"a" (intIn2), \
"r" (0) \
); \
})

// fsigned16 * fsigned16
// In the first adc, %A0 will never be 0xFF so there will be no carrry
// to propagate to %B0.
#define FMultiS16X16to16(intRes, intIn1, intIn2) \
({ \
char _tmp_; \
asm ( \
"fmuls %B2, %B3 \n\t" \
"movw %A0, r0 \n\t" \
"fmul %A2, %A3 \n\t" \
"adc %A0, %4 \n\t" \
"mov %1, r1 \n\t" \
"fmulsu %B3, %A2 \n\t" \
"sbc %B0, %4 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"fmulsu %B2, %A3 \n\t" \
"sbc %B0, %4 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (intRes), \
"=&r" (_tmp_) \
: \
"a" (intIn1), \
"a" (intIn2), \
"r" (0) \
); \
})

// fsigned16 * funsigned16 = fsigned16
// This is MultiSU16X16toH16 with a left shift from %1 into H16.
// It has been tested exhaustively.
#define FMultiSU16X16to16(intRes, intIn1, intIn2) \
({ \
char _tmp_; \
asm volatile ( \
"mul %A2, %A3 \n\t" \
"mov %1, r1 \n\t" \
"mulsu %B2, %B3 \n\t" \
"movw %A0, r0 \n\t" \
"mul %B3, %A2 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"mulsu %B2, %A3 \n\t" \
"sbc %B0, %4 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"lsl %1 \n\t" \
"rol %A0 \n\t" \
"rol %B0 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (intRes), \
"=&r" (_tmp_) \
: \
"a" (intIn1), \
"a" (intIn2), \
"r" (0) \
); \
})

// multiplies a signed and unsigned 16 bit ints with a 32 bit result
#define MultiSU16X16to32(longRes, intIn1, intIn2) \
asm ( \
"mul %A1, %A2 \n\t" \
"movw %A0, r0 \n\t" \
"mulsu %B1, %B2 \n\t" \
"movw %C0, r0 \n\t" \
"mul %B2, %A1 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, %4 \n\t" \
"mulsu %B1, %A2 \n\t" \
"sbc %D0, %3 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, %3 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (longRes), \
: \
"a" (intIn1), \
"a" (intIn2), \
"r" (0) \
)

// multiplies signed x unsigned int and returns the highest 16 bits of the result
#define MultiSU16X16toH16(intRes, intIn1, intIn2) \
({ \
char _tmp_; \
asm ( \
"mul %A2, %A3 \n\t" \
"mov %1, r1 \n\t" \
"mulsu %B2, %B3 \n\t" \
"movw %A0, r0 \n\t" \
"mul %B3, %A2 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"mulsu %B2, %A3 \n\t" \
"sbc %B0, %4 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (intRes), \
"=&r" (_tmp_) \
: \
"a" (intIn1), \
"a" (intIn2), \
"r" (0) \
); \
})

// multiplies signed x unsigned int and returns the highest 16 bits of the result
// rounds the result based on the MSB of the lower 16 bits
// 22 cycles
#define MultiSU16X16toH16Round(intRes, intIn1, intIn2) \
({ \
char _tmp_; \
asm ( \
"mul %A2, %A3 \n\t" \
"mov %1, r1 \n\t" \
"mulsu %B2, %B3 \n\t" \
"movw %A0, r0 \n\t" \
"mul %A2, %B3 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"mulsu %B2, %A3 \n\t" \
"sbc %B0, %4 \n\t" \
"add %1, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, %4 \n\t" \
"lsl %1 \n\t" \
"adc %A0, %4 \n\t" \
"adc %B0, %4 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (intRes), \
"=&r" (_tmp_) \
: \
"a" (intIn1), \
"a" (intIn2), \
"r" (0) \
); \
})
