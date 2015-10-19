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

  def self.next_number
    @number ||= 0
  ensure
    @number += 1
  end
end

class Constant
  def initialize(value:)
    @value = value
  end

  def create
    @value
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
        return accum += #{@delta.create};
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
        p->x = times_signed(p->x, #{@xscale.create});
        p->y = times_signed(p->y, #{@yscale.create});
      }
    }
    name
  end
end

def fix(n)
  "fix(#{n})"
end

name =
  Sum.new(
    term1: Quadrature.new(
      angle: Ramp.new(delta: 327),
      phase: Ramp.new(init: fix(0.5), delta: 1),
      fx: "zsin",
      fy: "zsin"),
    term2: Scale.new(
      term: Quadrature.new(
        angle: Ramp.new(delta: -360),
        phase: Ramp.new(init: fix(0.5), delta: 0),
        fx: "zsin",
        fy: "zsin"),
      xscale: fix(0.5))).create

puts "#define spiro #{name}"
