#!/usr/bin/env ruby

class Term
  def initialize(return_type, args)
    @return_type = return_type
    @args = args
    @name = "#{self.class.name.downcase}_#{self.class.next_number}"
  end

  def name
    @name
  end

  def declare
    %Q{
      static __inline__ #{@return_type}
      #{name}(#{@args})
      __attribute__((always_inline));
      
      static __inline__ #{@return_type}
      #{name}(#{@args})
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
    super("fix16_t", "void");
    @init = init
    @delta = maybe_constant(delta)
  end

  def create
    puts %Q{
      #{declare}
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
    super("void", "struct point *p");
    @angle = maybe_constant(angle)
    @phase = maybe_constant(phase)
    @fx = fx
    @fy = fy
  end

  def create
    puts %Q{
      #{declare}
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

name =
  Quadrature.new(
    angle: Ramp.new(delta: 327),
    phase: Ramp.new(init: "fix(0.5)", delta: 1),
    fx: "zsin",
    fy: "zsin").create

puts "#define spiro #{name}"
