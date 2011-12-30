#####################################################################
# Color class; wraps an sfml::Color* and provides cleanup on destruction
#####################################################################

.include "pir/wrapped.pir"

.namespace ['Color']

#Initialization code.
.sub 'init' :vtable
.end
.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create a Color with args."
  throw $P0
.end

#Reclaim this color.
.sub 'cleanup' :method
  .param pmc ptr
  GAME_DeleteColor(ptr)
.end

#TODO:
#  CLR_SetRed
#  CLR_SetGreen
#  CLR_SetBlue
#  CLR_SetAlpha
#  Make other classes Color-aware.
#  Re-work existing library code; consistent naming, etc.

#Initialize this class.
.sub Color_class_init :anon :load :init
  $P0 = get_class 'Wrapped'
  $P1 = newclass 'Color'
  addparent $P0, $P1
.end


