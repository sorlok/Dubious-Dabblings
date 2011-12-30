#####################################################################
# Color class; wraps an sfml::Color* and provides cleanup on destruction
#####################################################################

.namespace ['Color']
.sub 'init' :vtable
  #Calling init is always wrong.
  self.'bad_call'()
.end

.sub 'init_pmc' :vtable
  #Calling init_pmc only works if passed an UnManagedStruct
  .param pmc args
  $S0 = typeof args
  if $S0 == 'UnManagedStruct' goto construct
    self.'bad_call'()

construct:
  #TODO: How to "promote" an UnManagedStruct
.end

.sub bad_call :method
  $P0 = new 'Exception'
  $P0 = "Must create a new Color object with a single argument of type UnManagedStruct passed to the constructor."
  throw $P0
.end

.sub 'destroy' :vtable
  #Call delete, if not null
.end

.sub Color_class_init :anon :load :init
    $P0 = get_class 'UnManagedStruct'
    $P2 = newclass 'Color'
    addparent $P0, $P2
.end


