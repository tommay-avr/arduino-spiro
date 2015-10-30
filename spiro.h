#include "inline.h"

      
      INLINE(fix16_t, prg_00_circles_constant_0, (void))
    
      {
        return 327;
      }
    

      
      INLINE(fix16_t, prg_00_circles_ramp_0, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_00_circles_constant_0();
      }
    

      
      INLINE(fix16_t, prg_00_circles_constant_1, (void))
    
      {
        return 1;
      }
    

      
      INLINE(fix16_t, prg_00_circles_ramp_1, (void))
    
      {
        static fix16_t accum = fix(0.5);
        return accum += prg_00_circles_constant_1();
      }
    

      
      INLINE(void, prg_00_circles_quadrature_0, (struct point *p))
    
      {
        fix16_t angle = prg_00_circles_ramp_0();
        p->x = zsin(angle);
        fix16_t phase = prg_00_circles_ramp_1();
        p->y = zsin(angle + phase);
      }
    

      
      INLINE(fix16_t, prg_00_circles_constant_2, (void))
    
      {
        return -360;
      }
    

      
      INLINE(fix16_t, prg_00_circles_ramp_2, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_00_circles_constant_2();
      }
    

      
      INLINE(fix16_t, prg_00_circles_constant_3, (void))
    
      {
        return 0;
      }
    

      
      INLINE(fix16_t, prg_00_circles_ramp_3, (void))
    
      {
        static fix16_t accum = fix(0.5);
        return accum += prg_00_circles_constant_3();
      }
    

      
      INLINE(void, prg_00_circles_quadrature_1, (struct point *p))
    
      {
        fix16_t angle = prg_00_circles_ramp_2();
        p->x = zsin(angle);
        fix16_t phase = prg_00_circles_ramp_3();
        p->y = zsin(angle + phase);
      }
    

      
      INLINE(fix16_t, prg_00_circles_knob_0, (void))
    
      {
        int16_t value = adc_values[0];
        
            // [-1.0, 1.0)
            return value - 32768;
          
      }
    

      
      INLINE(void, prg_00_circles_scale_0, (struct point *p))
    
      {
        prg_00_circles_quadrature_1(p);
        fix16_t xscale = prg_00_circles_knob_0();
        p->x = times_signed(p->x, xscale);
        fix16_t yscale = xscale;
        p->y = times_signed(p->y, yscale);
      }
    

      
      INLINE(void, prg_00_circles_sum_0, (struct point *p))
    
      {
        prg_00_circles_quadrature_0(p);
        struct point p2;
        prg_00_circles_scale_0(&p2);
        p->x += p2.x;
        p->y += p2.y;
      }
    
#define prg_00_circles_main prg_00_circles_sum_0

      
      INLINE(fix16_t, prg_05_magnitude_constant_4, (void))
    
      {
        return 327;
      }
    

      
      INLINE(fix16_t, prg_05_magnitude_ramp_4, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_05_magnitude_constant_4();
      }
    

      
      INLINE(fix16_t, prg_05_magnitude_constant_5, (void))
    
      {
        return fix(0.5);
      }
    

      
      INLINE(void, prg_05_magnitude_quadrature_2, (struct point *p))
    
      {
        fix16_t angle = prg_05_magnitude_ramp_4();
        p->x = zsin(angle);
        fix16_t phase = prg_05_magnitude_constant_5();
        p->y = zsin(angle + phase);
      }
    

      
      INLINE(fix16_t, prg_05_magnitude_knob_1, (void))
    
      {
        int16_t value = adc_values[0];
        
            // [-1.0, 1.0)
            return value - 32768;
          
      }
    

      
      INLINE(void, prg_05_magnitude_scale_1, (struct point *p))
    
      {
        prg_05_magnitude_quadrature_2(p);
        fix16_t xscale = prg_05_magnitude_knob_1();
        p->x = times_signed(p->x, xscale);
        fix16_t yscale = xscale;
        p->y = times_signed(p->y, yscale);
      }
    
#define prg_05_magnitude_main prg_05_magnitude_scale_1

      
      INLINE(fix16_t, prg_06_phase_constant_6, (void))
    
      {
        return 327;
      }
    

      
      INLINE(fix16_t, prg_06_phase_ramp_5, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_06_phase_constant_6();
      }
    

      
      INLINE(fix16_t, prg_06_phase_knob_2, (void))
    
      {
        int16_t value = adc_values[0];
        
            // [-1.0, 1.0)
            return value - 32768;
          
      }
    

      
      INLINE(void, prg_06_phase_quadrature_3, (struct point *p))
    
      {
        fix16_t angle = prg_06_phase_ramp_5();
        p->x = zsin(angle);
        fix16_t phase = prg_06_phase_knob_2();
        p->y = zsin(angle + phase);
      }
    
#define prg_06_phase_main prg_06_phase_quadrature_3

      
      INLINE(fix16_t, prg_70_spinning_diamond_constant_7, (void))
    
      {
        return 327;
      }
    

      
      INLINE(fix16_t, prg_70_spinning_diamond_ramp_6, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_70_spinning_diamond_constant_7();
      }
    

      
      INLINE(fix16_t, prg_70_spinning_diamond_constant_8, (void))
    
      {
        return fix(0.5);
      }
    

      
      INLINE(void, prg_70_spinning_diamond_quadrature_4, (struct point *p))
    
      {
        fix16_t angle = prg_70_spinning_diamond_ramp_6();
        p->x = diamond(angle);
        fix16_t phase = prg_70_spinning_diamond_constant_8();
        p->y = diamond(angle + phase);
      }
    

      
      INLINE(fix16_t, prg_70_spinning_diamond_dda_0, (void))
    
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
    

      
      INLINE(void, prg_70_spinning_diamond_rotate_0, (struct point *p))
    
      {
        prg_70_spinning_diamond_quadrature_4(p);
        fix16_t angle = prg_70_spinning_diamond_dda_0();
        // What if we rotate something with a weird phase?  Probably need to
        // ensure s^2 + c^2 = 1.  Except what if they're both 0?
        fix16_t s = zsin(angle);
        fix16_t c = zcos(angle);
        fix16_t x = times_signed(p->x, c) - times_signed(p->y, s);
        fix16_t y = times_signed(p->x, s) + times_signed(p->y, c);
        p->x = x * 2;
        p->y = y * 2;
      }
    
#define prg_70_spinning_diamond_main prg_70_spinning_diamond_rotate_0

      
      INLINE(fix16_t, prg_71_spin_diamond_constant_9, (void))
    
      {
        return 327;
      }
    

      
      INLINE(fix16_t, prg_71_spin_diamond_ramp_7, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_71_spin_diamond_constant_9();
      }
    

      
      INLINE(fix16_t, prg_71_spin_diamond_constant_10, (void))
    
      {
        return fix(0.5);
      }
    

      
      INLINE(void, prg_71_spin_diamond_quadrature_5, (struct point *p))
    
      {
        fix16_t angle = prg_71_spin_diamond_ramp_7();
        p->x = diamond(angle);
        fix16_t phase = prg_71_spin_diamond_constant_10();
        p->y = diamond(angle + phase);
      }
    

      
      INLINE(fix16_t, prg_71_spin_diamond_knob_3, (void))
    
      {
        int16_t value = adc_values[1];
        
            // [-1.0, 1.0)
            return value - 32768;
          
      }
    

      
      INLINE(void, prg_71_spin_diamond_rotate_1, (struct point *p))
    
      {
        prg_71_spin_diamond_quadrature_5(p);
        fix16_t angle = prg_71_spin_diamond_knob_3();
        // What if we rotate something with a weird phase?  Probably need to
        // ensure s^2 + c^2 = 1.  Except what if they're both 0?
        fix16_t s = zsin(angle);
        fix16_t c = zcos(angle);
        fix16_t x = times_signed(p->x, c) - times_signed(p->y, s);
        fix16_t y = times_signed(p->x, s) + times_signed(p->y, c);
        p->x = x * 2;
        p->y = y * 2;
      }
    
#define prg_71_spin_diamond_main prg_71_spin_diamond_rotate_1

  #define NUM_PROGRAMS (5)
  static void (*programs[])(struct point *p) = {
    prg_00_circles_main, prg_05_magnitude_main, prg_06_phase_main, prg_70_spinning_diamond_main, prg_71_spin_diamond_main
  };
