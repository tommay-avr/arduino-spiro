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
        uint16_t value = adc_values[0];
        
            // [-1.0, 1.0)
            return value - 32768;
          
      }
    

      
      INLINE(void, prg_00_circles_scale2_0, (struct point *p))
    
      {
        prg_00_circles_quadrature_1(p);
        fix16_t xscale = prg_00_circles_knob_0();
        p->x = times_signed(p->x, xscale);
        fix16_t yscale = xscale;
        p->y = times_signed(p->y, yscale);
      }
    

      
      INLINE(void, prg_00_circles_sum2_0, (struct point *p))
    
      {
        prg_00_circles_quadrature_0(p);
        struct point p2;
        prg_00_circles_scale2_0(&p2);
        p->x += p2.x;
        p->y += p2.y;
      }
    
#define prg_00_circles_main prg_00_circles_sum2_0

      
      INLINE(fix16_t, prg_01_circle_knob_1, (void))
    
      {
        uint16_t value = adc_values[3];
        
            int16_t result;
            MultiU16X16toH16Round(result, value, 20 - 0);
            return result + 0;
          
      }
    

      
      INLINE(fix16_t, prg_01_circle_ramp_4, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_01_circle_knob_1();
      }
    

      
      INLINE(fix16_t, prg_01_circle_constant_4, (void))
    
      {
        return fix(0.5);
      }
    

      
      INLINE(void, prg_01_circle_quadrature_2, (struct point *p))
    
      {
        fix16_t angle = prg_01_circle_ramp_4();
        p->x = zsin(angle);
        fix16_t phase = prg_01_circle_constant_4();
        p->y = zsin(angle + phase);
      }
    
#define prg_01_circle_main prg_01_circle_quadrature_2

      
      INLINE(fix16_t, prg_02_spiro_knob_2, (void))
    
      {
        uint16_t value = adc_values[3];
        
            int16_t result;
            MultiU16X16toH16Round(result, value, 725 - 500);
            return result + 500;
          
      }
    

      
      INLINE(fix16_t, prg_02_spiro_ramp_5, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_02_spiro_knob_2();
      }
    

      
      INLINE(fix16_t, prg_02_spiro_constant_5, (void))
    
      {
        return fix(0.5);
      }
    

      
      INLINE(void, prg_02_spiro_quadrature_3, (struct point *p))
    
      {
        fix16_t angle = prg_02_spiro_ramp_5();
        p->x = zsin(angle);
        fix16_t phase = prg_02_spiro_constant_5();
        p->y = zsin(angle + phase);
      }
    

      
      INLINE(fix16_t, prg_02_spiro_knob_3, (void))
    
      {
        uint16_t value = adc_values[0];
        
            // [-1.0, 1.0)
            return value - 32768;
          
      }
    

      
      INLINE(void, prg_02_spiro_scale2_1, (struct point *p))
    
      {
        prg_02_spiro_quadrature_3(p);
        fix16_t xscale = prg_02_spiro_knob_3();
        p->x = times_signed(p->x, xscale);
        fix16_t yscale = xscale;
        p->y = times_signed(p->y, yscale);
      }
    

      
      INLINE(fix16_t, prg_02_spiro_constant_6, (void))
    
      {
        return -360;
      }
    

      
      INLINE(fix16_t, prg_02_spiro_ramp_6, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_02_spiro_constant_6();
      }
    

      
      INLINE(fix16_t, prg_02_spiro_constant_7, (void))
    
      {
        return fix(0.5);
      }
    

      
      INLINE(void, prg_02_spiro_quadrature_4, (struct point *p))
    
      {
        fix16_t angle = prg_02_spiro_ramp_6();
        p->x = zsin(angle);
        fix16_t phase = prg_02_spiro_constant_7();
        p->y = zsin(angle + phase);
      }
    

      
      INLINE(fix16_t, prg_02_spiro_knob_4, (void))
    
      {
        uint16_t value = adc_values[1];
        
            // [-1.0, 1.0)
            return value - 32768;
          
      }
    

      
      INLINE(fix16_t, prg_02_spiro_constant_8, (void))
    
      {
        return fix(1);
      }
    

      
      INLINE(fix16_t, prg_02_spiro_knob_5, (void))
    
      {
        uint16_t value = adc_values[4];
        
            int16_t result;
            MultiU16X16toH16Round(result, value, 19 - 0);
            return result + 0;
          
      }
    

      
      INLINE(fix16_t, prg_02_spiro_ramp_7, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_02_spiro_knob_5();
      }
    

      
      INLINE(fix16_t, prg_02_spiro_sin_0, (void))
    
      {
        return zsin(prg_02_spiro_ramp_7()) * 2;
      }
    

      
      INLINE(fix16_t, prg_02_spiro_mix_0, (void))
    
      {
        // Mix: [-1.0, 1.0):
        // -1: 1.0*term1 + 0.0*term2
        //  0: 0.5*tern1 + 0.5*term2
        //  1: 0.0*term1 + 1.0*term2
        //  (1-mix)/2 * term1 + (1+mix)/2 * term2
        fix16_t mix = prg_02_spiro_knob_4();
        fix16_t term1 = prg_02_spiro_constant_8();
        fix16_t term2 = prg_02_spiro_sin_0();
        return times_signed((ufix16_t)(fix(1) - mix - 1) >> 1, term1) +
               times_signed((ufix16_t)(fix(1) + mix) >> 1, term2);
      }
    

      
      INLINE(void, prg_02_spiro_scale2_2, (struct point *p))
    
      {
        prg_02_spiro_quadrature_4(p);
        fix16_t xscale = prg_02_spiro_mix_0();
        p->x = times_signed(p->x, xscale);
        fix16_t yscale = xscale;
        p->y = times_signed(p->y, yscale);
      }
    

      
      INLINE(void, prg_02_spiro_sum2_1, (struct point *p))
    
      {
        prg_02_spiro_scale2_1(p);
        struct point p2;
        prg_02_spiro_scale2_2(&p2);
        p->x += p2.x;
        p->y += p2.y;
      }
    
#define prg_02_spiro_main prg_02_spiro_sum2_1

      
      INLINE(fix16_t, prg_05_magnitude_constant_9, (void))
    
      {
        return 327;
      }
    

      
      INLINE(fix16_t, prg_05_magnitude_ramp_8, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_05_magnitude_constant_9();
      }
    

      
      INLINE(fix16_t, prg_05_magnitude_constant_10, (void))
    
      {
        return fix(0.5);
      }
    

      
      INLINE(void, prg_05_magnitude_quadrature_5, (struct point *p))
    
      {
        fix16_t angle = prg_05_magnitude_ramp_8();
        p->x = zsin(angle);
        fix16_t phase = prg_05_magnitude_constant_10();
        p->y = zsin(angle + phase);
      }
    

      
      INLINE(fix16_t, prg_05_magnitude_knob_6, (void))
    
      {
        uint16_t value = adc_values[0];
        
            // [-1.0, 1.0)
            return value - 32768;
          
      }
    

      
      INLINE(void, prg_05_magnitude_scale2_3, (struct point *p))
    
      {
        prg_05_magnitude_quadrature_5(p);
        fix16_t xscale = prg_05_magnitude_knob_6();
        p->x = times_signed(p->x, xscale);
        fix16_t yscale = xscale;
        p->y = times_signed(p->y, yscale);
      }
    
#define prg_05_magnitude_main prg_05_magnitude_scale2_3

      
      INLINE(fix16_t, prg_06_phase_constant_11, (void))
    
      {
        return 327;
      }
    

      
      INLINE(fix16_t, prg_06_phase_ramp_9, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_06_phase_constant_11();
      }
    

      
      INLINE(fix16_t, prg_06_phase_knob_7, (void))
    
      {
        uint16_t value = adc_values[0];
        
            // [-1.0, 1.0)
            return value - 32768;
          
      }
    

      
      INLINE(void, prg_06_phase_quadrature_6, (struct point *p))
    
      {
        fix16_t angle = prg_06_phase_ramp_9();
        p->x = zsin(angle);
        fix16_t phase = prg_06_phase_knob_7();
        p->y = zsin(angle + phase);
      }
    
#define prg_06_phase_main prg_06_phase_quadrature_6

      
      INLINE(fix16_t, prg_70_spinning_diamond_constant_12, (void))
    
      {
        return 327;
      }
    

      
      INLINE(fix16_t, prg_70_spinning_diamond_ramp_10, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_70_spinning_diamond_constant_12();
      }
    

      
      INLINE(fix16_t, prg_70_spinning_diamond_constant_13, (void))
    
      {
        return fix(0.5);
      }
    

      
      INLINE(void, prg_70_spinning_diamond_quadrature_7, (struct point *p))
    
      {
        fix16_t angle = prg_70_spinning_diamond_ramp_10();
        p->x = diamond(angle);
        fix16_t phase = prg_70_spinning_diamond_constant_13();
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
        prg_70_spinning_diamond_quadrature_7(p);
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

      
      INLINE(fix16_t, prg_71_spin_diamond_constant_14, (void))
    
      {
        return 327;
      }
    

      
      INLINE(fix16_t, prg_71_spin_diamond_ramp_11, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_71_spin_diamond_constant_14();
      }
    

      
      INLINE(fix16_t, prg_71_spin_diamond_constant_15, (void))
    
      {
        return fix(0.5);
      }
    

      
      INLINE(void, prg_71_spin_diamond_quadrature_8, (struct point *p))
    
      {
        fix16_t angle = prg_71_spin_diamond_ramp_11();
        p->x = diamond(angle);
        fix16_t phase = prg_71_spin_diamond_constant_15();
        p->y = diamond(angle + phase);
      }
    

      
      INLINE(fix16_t, prg_71_spin_diamond_knob_8, (void))
    
      {
        uint16_t value = adc_values[1];
        
            // [-1.0, 1.0)
            return value - 32768;
          
      }
    

      
      INLINE(void, prg_71_spin_diamond_rotate_1, (struct point *p))
    
      {
        prg_71_spin_diamond_quadrature_8(p);
        fix16_t angle = prg_71_spin_diamond_knob_8();
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

      
      INLINE(fix16_t, prg_72_2xspin_diamond_constant_16, (void))
    
      {
        return 327;
      }
    

      
      INLINE(fix16_t, prg_72_2xspin_diamond_ramp_12, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_72_2xspin_diamond_constant_16();
      }
    

      
      INLINE(fix16_t, prg_72_2xspin_diamond_constant_17, (void))
    
      {
        return fix(0.5);
      }
    

      
      INLINE(void, prg_72_2xspin_diamond_quadrature_9, (struct point *p))
    
      {
        fix16_t angle = prg_72_2xspin_diamond_ramp_12();
        p->x = diamond(angle);
        fix16_t phase = prg_72_2xspin_diamond_constant_17();
        p->y = diamond(angle + phase);
      }
    

      
      INLINE(fix16_t, prg_72_2xspin_diamond_knob_9, (void))
    
      {
        uint16_t value = adc_values[1];
        
            // [-1.0, 1.0)
            return value - 32768;
          
      }
    

      
      INLINE(fix16_t, prg_72_2xspin_diamond_constant_18, (void))
    
      {
        return 2;
      }
    

      
      INLINE(fix16_t, prg_72_2xspin_diamond_scale1_0, (void))
    
      {
        return times_signed(prg_72_2xspin_diamond_knob_9(), prg_72_2xspin_diamond_constant_18());
      }
    

      
      INLINE(void, prg_72_2xspin_diamond_rotate_2, (struct point *p))
    
      {
        prg_72_2xspin_diamond_quadrature_9(p);
        fix16_t angle = prg_72_2xspin_diamond_scale1_0();
        // What if we rotate something with a weird phase?  Probably need to
        // ensure s^2 + c^2 = 1.  Except what if they're both 0?
        fix16_t s = zsin(angle);
        fix16_t c = zcos(angle);
        fix16_t x = times_signed(p->x, c) - times_signed(p->y, s);
        fix16_t y = times_signed(p->x, s) + times_signed(p->y, c);
        p->x = x * 2;
        p->y = y * 2;
      }
    
#define prg_72_2xspin_diamond_main prg_72_2xspin_diamond_rotate_2

      
      INLINE(fix16_t, prg_80_phase_diamond_constant_19, (void))
    
      {
        return 327;
      }
    

      
      INLINE(fix16_t, prg_80_phase_diamond_ramp_13, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_80_phase_diamond_constant_19();
      }
    

      
      INLINE(fix16_t, prg_80_phase_diamond_knob_10, (void))
    
      {
        uint16_t value = adc_values[4];
        
            // [-1.0, 1.0)
            return value - 32768;
          
      }
    

      
      INLINE(void, prg_80_phase_diamond_quadrature_10, (struct point *p))
    
      {
        fix16_t angle = prg_80_phase_diamond_ramp_13();
        p->x = diamond(angle);
        fix16_t phase = prg_80_phase_diamond_knob_10();
        p->y = diamond(angle + phase);
      }
    
#define prg_80_phase_diamond_main prg_80_phase_diamond_quadrature_10

      
      INLINE(fix16_t, prg_81_phased_diamond_constant_20, (void))
    
      {
        return 327;
      }
    

      
      INLINE(fix16_t, prg_81_phased_diamond_ramp_14, (void))
    
      {
        static fix16_t accum ;
        return accum += prg_81_phased_diamond_constant_20();
      }
    

      
      INLINE(fix16_t, prg_81_phased_diamond_dda_1, (void))
    
      {
        static fix16_t n = 0;
        static int16_t error = 0;

        error += 100;
        if (error >= 0) {
          error -= 200;
          n++;
        }
        return n;
      }
    

      
      INLINE(void, prg_81_phased_diamond_quadrature_11, (struct point *p))
    
      {
        fix16_t angle = prg_81_phased_diamond_ramp_14();
        p->x = diamond(angle);
        fix16_t phase = prg_81_phased_diamond_dda_1();
        p->y = diamond(angle + phase);
      }
    

      
      INLINE(fix16_t, prg_81_phased_diamond_dda_2, (void))
    
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
    

      
      INLINE(void, prg_81_phased_diamond_rotate_3, (struct point *p))
    
      {
        prg_81_phased_diamond_quadrature_11(p);
        fix16_t angle = prg_81_phased_diamond_dda_2();
        // What if we rotate something with a weird phase?  Probably need to
        // ensure s^2 + c^2 = 1.  Except what if they're both 0?
        fix16_t s = zsin(angle);
        fix16_t c = zcos(angle);
        fix16_t x = times_signed(p->x, c) - times_signed(p->y, s);
        fix16_t y = times_signed(p->x, s) + times_signed(p->y, c);
        p->x = x * 2;
        p->y = y * 2;
      }
    
#define prg_81_phased_diamond_main prg_81_phased_diamond_rotate_3

  #define NUM_PROGRAMS (10)
  static void (*programs[])(struct point *p) = {
    prg_00_circles_main, prg_01_circle_main, prg_02_spiro_main, prg_05_magnitude_main, prg_06_phase_main, prg_70_spinning_diamond_main, prg_71_spin_diamond_main, prg_72_2xspin_diamond_main, prg_80_phase_diamond_main, prg_81_phased_diamond_main
  };
