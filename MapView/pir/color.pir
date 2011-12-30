#####################################################################
# Color class; wraps an sfml::Color* and provides cleanup on destruction
#####################################################################

.namespace ['Color']
.sub 'init' :vtable
.end

.sub 'set_ptr' :method
  .param pmc otherPtr
  setattribute self, 'ptr', otherPtr
.end

.sub 'get_ptr' :method
  .local pmc ptr
  ptr = getattribute self, 'ptr'
  .return(ptr)
.end

.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create a Color with args."
  throw $P0
.end

.sub 'destroy' :vtable
  .local pmc ptr
  ptr = getattribute self, 'ptr'
  unless ptr goto done
  GAME_DeleteColor(ptr)
  say "Color reclaimed"
done:
.end

.sub Color_class_init :anon :load :init
  $P0 = newclass 'Color'
  addattribute $P0, 'ptr'
.end


