
      
      static __inline__ fix16_t
      ramp_0(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      ramp_0(void)
    
      {
        static fix16_t accum ;
        return accum += 327;
      }
    

      
      static __inline__ fix16_t
      ramp_1(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      ramp_1(void)
    
      {
        static fix16_t accum = fix(0.5);
        return accum += 1;
      }
    

      
      static __inline__ void
      quadrature_0(struct point *p)
      __attribute__((always_inline));
      
      static __inline__ void
      quadrature_0(struct point *p)
    
      {
        fix16_t angle = ramp_0();
        p->x = zsin(angle);
        fix16_t phase = ramp_1();
        p->y = zsin(angle + phase);
      }
    
#define spiro quadrature_0
