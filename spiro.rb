#!/usr/bin/env ruby

class Term
  def initialize
    @name = "#{prefix}_#{self.class.name.downcase}_#{self.class.next_number}"
  end

  def create(code)
    puts %Q{
      #{declare}
      {
        #{code}
      }
    }
    @name
  end

  def declare(return_type, args)
    %Q{
      INLINE(#{return_type}, #{@name}, (#{args}))
    }
  end

  def maybe_constant(term)
    case term
    when Fixnum, String
      Constant.new(value: term)
    else
      term
    end
  end

  def self.next_number
    @number ||= 0
  ensure
    @number += 1
  end
end

class Term1 < Term
  def initialize
    super()
  end

  def declare
    super("fix16_t", "void")
  end

  def +(term)
    Sum1.new(term1: self, term2: term)
  end

  def *(scale)
    Scale1.new(term: self, scale: scale)
  end
end

class Term2 < Term
  def initialize
    super()
  end

  def declare
    super("void", "struct point *p")
  end

  def +(term)
    Sum2.new(term1: self, term2: term)
  end

  def *(scale)
    case scale
    when Array
      Scale2.new(term: self, xscale: scale[0], yscale: scale[1])
    else
      Scale2.new(term: self, xscale: scale)
    end
  end

  def %(angle)
    Rotate.new(term: self, angle: angle)
  end
end

class Knob < Term1
  def initialize(channel, lo: nil, hi: nil)
    super()
    @channel = channel
    @lo = lo
    @hi = hi
  end

  def create
    super %Q{
      uint16_t value = adc_values[#{@channel}];
      #{(@lo && @hi) ?
        %Q{
          int16_t result;
          MultiU16X16toH16Round(result, value, #{@hi} - #{@lo});
          return result + #{@lo};
        } :
        %Q{
          // [-1.0, 1.0)
          return value - 32768;
        }
      }
    }
  end
end

def knob(*args)
  Knob.new(*args)
end

class Constant < Term1
  def initialize(value:)
    super()
    @value = value
  end

  def create
    super %Q{
      return #{@value};
    }
  end
end

class Ramp < Term1
  def initialize(init: 0, delta:)
    super()
    @init = init
    @delta = maybe_constant(delta)
  end

  def create
    super %Q{
      static fix16_t accum #{@init != 0 ? "= #{@init}" : ""};
      return accum += #{@delta.create}();
    }
  end
end

def ramp(*args)
  Ramp.new(*args)
end

class DDA < Term1
  def initialize(n:, ticks:)
    super()
    @n = n;
    @ticks = ticks;
  end

  def create
    super %Q{
      static fix16_t n = 0;
      static int16_t error = 0;

      error += #{@n};
      if (error >= 0) {
        error -= #{@ticks};
        n++;
      }
      return n;
    }
  end
end

def dda(*args)
  DDA.new(*args)
end

class Quadrature < Term2
  def initialize(angle:, phase: "fix(0.5)", fx:, fy:)
    super()
    @angle = maybe_constant(angle)
    @phase = maybe_constant(phase)
    @fx = fx
    @fy = fy
  end

  def create
    super %Q{
      fix16_t angle = #{@angle.create}();
      p->x = #{@fx}(angle);
      fix16_t phase = #{@phase.create}();
      p->y = #{@fy}(angle + phase);
    }
  end
end

def quadrature(*args)
  Quadrature.new(*args)
end

def circle(angle:, phase: "fix(0.5)")
  quadrature(angle: angle, phase: phase, fx: "zsin", fy: "zsin")
end

def diamond(angle:, phase: "fix(0.5)")
  quadrature(angle: angle, phase: phase, fx: "diamond", fy: "diamond")
end

class Lissajous < Term2
  def initialize(x:, y:)
    super()
    @x = x
    @y = y
  end

  def create
    super %Q{
      p->x = #{@x.create}();
      p->y = #{@y.create}();
    }
  end
end

def lissajous(*args)
  Lissajous.new(*args)
end

class Sum1 < Term1
  def initialize(term1:, term2:)
    super()
    @term1 = term1
    @term2 = term2
  end

  def create
    super %Q{
      return #{@term1.create}() + #{@term2.create}();
    }
  end
end

class Sum2 < Term2
  def initialize(term1:, term2:)
    super()
    @term1 = term1
    @term2 = term2
  end

  def create
    super %Q{
      #{@term1.create}(p);
      struct point p2;
      #{@term2.create}(&p2);
      p->x += p2.x;
      p->y += p2.y;
    }
  end
end

class Scale1 < Term1
  def initialize(term:, scale:)
    super()
    @term = term
    @scale = maybe_constant(scale)
  end

  def create
    super %Q{
      return times_signed(#{@term.create}(), #{@scale.create}());
    }
  end
end

class Scale2 < Term2
  def initialize(term:, xscale:, yscale: nil)
    super()
    @term = term
    @xscale = maybe_constant(xscale)
    @yscale = yscale && maybe_constant(yscale)
  end

  def create
    super %Q{
      #{@term.create}(p);
      fix16_t xscale = #{@xscale.create}();
      p->x = times_signed(p->x, xscale);
      fix16_t yscale = #{@yscale ? "#{@yscale.create}()" : "xscale"};
      p->y = times_signed(p->y, yscale);
    }
  end
end

class Rotate < Term2
  def initialize(term:, angle:)
    super()
    @term = term
    @angle = maybe_constant(angle)
  end

  def create
    super %Q{
      #{@term.create}(p);
      fix16_t angle = #{@angle.create}();
      // What if we rotate something with a weird phase?  Probably need to
      // ensure s^2 + c^2 = 1.  Except what if they're both 0?
      fix16_t s = zsin(angle);
      fix16_t c = zcos(angle);
      fix16_t x = times_signed(p->x, c) - times_signed(p->y, s);
      fix16_t y = times_signed(p->x, s) + times_signed(p->y, c);
      p->x = x * 2;
      p->y = y * 2;
    }
  end
end

class Sin < Term1
  def initialize(term:)
    super()
    @term = term
  end

  def create
    super %Q{
      return zsin(#{@term.create}()) * 2;
    }
  end
end

def sin(term)
  Sin.new(term: term)
end

class Mix < Term1
  def initialize(mix:, term1:, term2:)
    super()
    @mix = mix
    @term1 = maybe_constant(term1)
    @term2 = maybe_constant(term2)
  end

  def create
    super %Q{
      // Mix: [-1.0, 1.0):
      // -1: 1.0*term1 + 0.0*term2
      //  0: 0.5*tern1 + 0.5*term2
      //  1: 0.0*term1 + 1.0*term2
      //  (1-mix)/2 * term1 + (1+mix)/2 * term2
      fix16_t mix = #{@mix.create}();
      fix16_t term1 = #{@term1.create}();
      fix16_t term2 = #{@term2.create}();
      return times_signed((ufix16_t)(fix(1) - mix - 1) >> 1, term1) +
             times_signed((ufix16_t)(fix(1) + mix) >> 1, term2);
    }
  end
end

def mix(*args)
  Mix.new(*args)
end

def fix(n)
  "fix(#{n})"
end

puts %Q{#include "inline.h"}

programs = ARGV.map do |filename|
  basename = File.basename(filename, ".spiro")
  prefix = "prg_#{basename}".gsub(/-/, "_")

  # prefix is a "global" method that lets the initializers include the
  # filename into their object's names without having to pass it
  # around everywhere.  Nasty but easy.
  #
  define_method(:prefix) do
    prefix
  end

  main = eval(File.read(filename)).create
  puts "#define #{prefix}_main #{main}"

  "#{prefix}_main"
end

puts %Q{
  static void (*programs[])(struct point *p) = {
    #{programs.join(", ")}
  };
}
