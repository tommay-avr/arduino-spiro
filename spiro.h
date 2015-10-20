
      
      static __inline__ fix16_t
      constant_6(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      constant_6(void)
    
      {
        return 327;
      }
    

      
      static __inline__ fix16_t
      ramp_4(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      ramp_4(void)
    
      {
        static fix16_t accum ;
        return accum += constant_6();
      }
    

      
      static __inline__ fix16_t
      constant_7(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      constant_7(void)
    
      {
        return fix(0.5);
      }
    

      
      static __inline__ void
      diamond_0(struct point *p)
      __attribute__((always_inline));
      
      static __inline__ void
      diamond_0(struct point *p)
    
      {
        fix16_t angle = ramp_4();
        p->x = diamond(angle);
        fix16_t phase = constant_7();
        p->y = diamond(angle + phase);
      }
    

      
      static __inline__ fix16_t
      dda_0(void)
      __attribute__((always_inline));
      
      static __inline__ fix16_t
      dda_0(void)
    
      {
        static fix16_t n = 0;
        static int16_t error = 0;

        error += fix(0.1);
        if (error >= 0) {
          error -= 1000;
          n++;
        }
        return n;
      }
    

      
      static __inline__ void
      rotate_0(struct point *p)
      __attribute__((always_inline));
      
      static __inline__ void
      rotate_0(struct point *p)
    
      {
        diamond_0(p);
        fix16_t angle = dda_0();
        // What if we rotate something with a weird phase?  Probably need to
        // ensure s^2 + c^2 = 1.  Except what if they're both 0?
        fix16_t s = zsin(angle);
        fix16_t c = zcos(angle);
        fix16_t x = times_signed(p->x, c) - times_signed(p->y, s);
        fix16_t y = times_signed(p->x, s) + times_signed(p->y, c);
        p->x = x * 2;
        p->y = y * 2;
      }
    

      
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
      circle_0(struct point *p)
      __attribute__((always_inline));
      
      static __inline__ void
      circle_0(struct point *p)
    
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
      circle_1(struct point *p)
      __attribute__((always_inline));
      
      static __inline__ void
      circle_1(struct point *p)
    
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
        circle_1(p);
        p->x = times_signed(p->x, constant_4());
        p->y = times_signed(p->y, constant_5());
      }
    

      
      static __inline__ void
      sum_0(struct point *p)
      __attribute__((always_inline));
      
      static __inline__ void
      sum_0(struct point *p)
    
      {
        circle_0(p);
        struct point p2;
        scale_0(&p2);
        p->x += p2.x;
        p->y += p2.y;
      }
    
#define spiro sum_0
