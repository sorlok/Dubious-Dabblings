#Include required code
#include "pir/interfaces.pir"


#Subclass GameObj
#NOTE: Subclasses aren't working right in cardinal, but perhaps we can 
#      just "pretend" that it's a subclass?
class SpinningPerson #< Sprite 
  def initialize()
    @rotation = 0.0
  end 

  def update(elapsed) 
    #Spin cursor
    @rotation = 50*elapsed + @rotation
    return @rotation
  end
end

#Test...
#pers = SpinningPerson.new
#puts pers.update(0.1)
#puts pers.update(0.1)
#puts pers.update(0.1)


