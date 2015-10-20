
      
      static __inline__ fix16_t
      constant_0(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      constant_0(void)
    
      {
        return 327;
      }
    

      
      static __inline__ fix16_t
      ramp_0(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      ramp_0(void)
    
      {
        static fix16_t accum ;
        return accum += constant_0();
      }
    

      
      static __inline__ fix16_t
      constant_1(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      constant_1(void)
    
      {
        return 1;
      }
    

      
      static __inline__ fix16_t
      ramp_1(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      ramp_1(void)
    
      {
        static fix16_t accum = fix(0.5);
        return accum += constant_1();
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
    

      
      static __inline__ fix16_t
      constant_2(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      constant_2(void)
    
      {
        return -360;
      }
    

      
      static __inline__ fix16_t
      ramp_2(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      ramp_2(void)
    
      {
        static fix16_t accum ;
        return accum += constant_2();
      }
    

      
      static __inline__ fix16_t
      constant_3(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      constant_3(void)
    
      {
        return 0;
      }
    

      
      static __inline__ fix16_t
      ramp_3(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      ramp_3(void)
    
      {
        static fix16_t accum = fix(0.5);
        return accum += constant_3();
      }
    

      
      static __inline__ void
      quadrature_1(struct point *p)
      __attribute__((always_inline));
      
      static __inline__ void
      quadrature_1(struct point *p)
    
      {
        fix16_t angle = ramp_2();
        p->x = zsin(angle);
        fix16_t phase = ramp_3();
        p->y = zsin(angle + phase);
      }
    

      
      static __inline__ fix16_t
      constant_4(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      constant_4(void)
    
      {
        return fix(0.5);
      }
    

      
      static __inline__ fix16_t
      constant_5(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      constant_5(void)
    
      {
        return fix(0.5);
      }
    

      
      static __inline__ void
      scale_0(struct point *p)
      __attribute__((always_inline));
      
      static __inline__ void
      scale_0(struct point *p)
    
      {
        quadrature_1(p);
        p->x = times_signed(p->x, constant_4());
        p->y = times_signed(p->y, constant_5());
      }
    

      
      static __inline__ void
      sum_0(struct point *p)
      __attribute__((always_inline));
      
      static __inline__ void
      sum_0(struct point *p)
    
      {
        quadrature_0(p);
        struct point p2;
        scale_0(&p2);
        p->x += p2.x;
        p->y += p2.y;
      }
    
#define spiro sum_0
