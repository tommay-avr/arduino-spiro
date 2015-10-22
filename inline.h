#ifndef _inline_h_
#define _inline_h_

#define INLINE(TYPE, NAME, ARGS) \
  static __inline__ TYPE \
  NAME ARGS \
  __attribute__((always_inline)); \
    \
  static __inline__ TYPE \
  NAME ARGS \

#define NO_INLINE(TYPE, NAME, ARGS) \
  static __inline__ TYPE \
  NAME ARGS \
  __attribute__((noinline)); \
    \
  static __inline__ TYPE \
  NAME ARGS \

#endif /* ndef _inline_h_ */
