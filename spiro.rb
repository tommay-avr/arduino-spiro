#!/usr/bin/env ruby

class Term
  def initialize
    @name = "#{self.class.name.downcase}_#{self.class.next_number}"
  end

  def name
    @name
  end

  def declare(return_type, args)
    %Q{
      static __inline__ #{return_type}
      #{name}(#{args})
      __attribute__((always_inline));
      
      static __inline__ #{return_type}
      #{name}(#{args})
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
      static fix16_t #{name}_accum #{@init != 0 ? "= #{@init}" : ""};

      #{declare("fix16_t", "void")}
      {
        return #{name}_accum += #{@delta.create}();
      }
    }
    name
  end
end

class DDA < Term
  def initialize(n:, ticks:)
    super()
    @n = n;
    @ticks = ticks;
  end

  def create
    puts %Q{
      static fix16_t #{name}_n = 0;
      static int16_t #{name}_error = 0;

      #{declare("fix16_t", "void")}
      {
        #{name}_error += #{@n};
        if (#{name}_error >= 0) {
          #{name}_error -= #{@ticks};
          #{name}_n++;
        }
        return #{name}_n;
      }
    }
    name
  end
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
    @yscale = maybe_constant(yscale || xscale)
  end

  def create
    puts %Q{
      #{declare("void", "struct point *p")}
      {
        #{@term.create}(p);
        p->x = times_signed(p->x, #{@xscale.create}());
        p->y = times_signed(p->y, #{@yscale.create}());
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

spiro =
  Quadrature.new(
    angle: Ramp.new(delta: 327),
    phase: Ramp.new(init: fix(0.5), delta: 1),
    fx: "zsin",
    fy: "zsin") +
  Quadrature.new(
    angle: Ramp.new(delta: -360),
    phase: Ramp.new(init: fix(0.5), delta: 0),
    fx: "zsin",
    fy: "zsin") * fix(0.5) # % DDA.new(n: fix(0.1), ticks: 1000)

name = spiro.create
puts "#define spiro #{name}"
