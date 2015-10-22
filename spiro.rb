#!/usr/bin/env ruby

class Term
  def initialize
    @name = "#{prefix}_#{self.class.name.downcase}_#{self.class.next_number}"
  end

  def name
    @name
  end

  def declare(return_type, args)
    %Q{
      INLINE(#{return_type}, #{name}, (#{args}))
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

  def +(term)
    Sum.new(term1: self, term2: term)
  end

  def *(scale)
    case scale
    when Array
      Scale.new(term: self, xscale: scale[0], yscale: scale[1])
    else
      Scale.new(term: self, xscale: scale)
    end
  end

  def %(angle)
    Rotate.new(term: self, angle: angle)
  end

  def self.next_number
    @number ||= 0
  ensure
    @number += 1
  end
end

class Constant < Term
  def initialize(value:)
    super()
    @value = value
  end

  def create
    puts %Q{
      #{declare("fix16_t", "void")}
      {
        return #{@value};
      }
    }
    name
  end
end

class Ramp < Term
  def initialize(init: 0, delta:)
    super()
    @init = init
    @delta = maybe_constant(delta)
  end

  def create
    puts %Q{
      #{declare("fix16_t", "void")}
      {
        static fix16_t accum #{@init != 0 ? "= #{@init}" : ""};
        return accum += #{@delta.create}();
      }
    }
    name
  end
end

def ramp(*args)
  Ramp.new(*args)
end

class DDA < Term
  def initialize(n:, ticks:)
    super()
    @n = n;
    @ticks = ticks;
  end

  def create
    puts %Q{
      #{declare("fix16_t", "void")}
      {
        static fix16_t n = 0;
        static int16_t error = 0;

        error += #{@n};
        if (error >= 0) {
          error -= #{@ticks};
          n++;
        }
        return n;
      }
    }
    name
  end
end

def dda(*args)
  DDA.new(*args)
end

class Quadrature < Term
  def initialize(angle:, phase: "fix(0.5)", fx:, fy:)
    super()
    @angle = maybe_constant(angle)
    @phase = maybe_constant(phase)
    @fx = fx
    @fy = fy
  end

  def create
    puts %Q{
      #{declare("void", "struct point *p")}
      {
        fix16_t angle = #{@angle.create}();
        p->x = #{@fx}(angle);
        fix16_t phase = #{@phase.create}();
        p->y = #{@fy}(angle + phase);
      }
    }
    name
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

class Sum < Term
  def initialize(term1:, term2:)
    super()
    @term1 = term1
    @term2 = term2
  end

  def create
    puts %Q{
      #{declare("void", "struct point *p")}
      {
        #{@term1.create}(p);
        struct point p2;
        #{@term2.create}(&p2);
        p->x += p2.x;
        p->y += p2.y;
      }
    }
    name
  end
end

class Scale < Term
  def initialize(term:, xscale:, yscale: nil)
    super()
    @term = term
    @xscale = maybe_constant(xscale)
    @yscale = yscale && maybe_constant(yscale)
  end

  def create
    puts %Q{
      #{declare("void", "struct point *p")}
      {
        #{@term.create}(p);
        fix16_t xscale = #{@xscale.create}();
        p->x = times_signed(p->x, xscale);
        fix16_t yscale = #{@yscale ? "#{@yscale.create}()" : "xscale"};
        p->y = times_signed(p->y, yscale);
      }
    }
    name
  end
end

class Rotate < Term
  def initialize(term:, angle:)
    super()
    @term = term
    @angle = maybe_constant(angle)
  end

  def create
    puts %Q{
      #{declare("void", "struct point *p")}
      {
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
    }
    name
  end
end

def fix(n)
  "fix(#{n})"
end

puts %Q{#include "inline.h"}

ARGV.each do |filename|
  basename = File.basename(filename, ".spiro")

  # prefix is a "global" method that lets the initializers include the
  # filename into their object's names without having to pass it
  # around everywhere.  Nasty but easy.
  #
  define_method(:prefix) do
    basename
  end

  main = eval(File.read(filename)).create
  puts "#define #{prefix}_main #{main}"
end
