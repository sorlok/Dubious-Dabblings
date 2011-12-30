#####################################################################
# Color class; wraps an sfml::Color* and provides cleanup on destruction
#####################################################################

.namespace ['Color']
.sub 'init' :vtable
.end

.sub 'set_ptr' :method
  .param pmc otherPtr
  self = otherPtr
.end

.sub 'init_pmc' :vtable
  self.'bad_call'() #Temp
 
  #Note: perhaps we can use the superclass directly?
  #Three arguments requried:
  # 0-The datatype. See the enum in include/parrot/datatypes.h.
  # 1-The count.
  # 2-The offset.

  #Calling init_pmc only works if passed an UnManagedStruct
  .param pmc args
  .local pmc ums
  $S0 = typeof args
  unless $S0 == 'ResizablePMCArray' goto bad_args
say "ok1"
  #Check there's only one
  $I0 = args
  unless $I0 == 1 goto bad_args
say "ok2"
  #TODO: How to "promote" an UnManagedStruct
  self = args
  
  #Done/error
  goto done
bad_args:
  self.'bad_call'()
done:
.end

.sub bad_call :method
  $P0 = new 'Exception'
  $P0 = "Must create a new Color object with a single argument of type ResizablePMCArray, containing a single element of type UnManagedStruct passed to the constructor."
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


