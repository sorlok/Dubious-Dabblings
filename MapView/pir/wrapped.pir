#####################################################################
# Wrapped class; generic wrapper for any UnManagedStruct which
#   must be closed later.
# cleanup(p) - Called on destruction if the wrapped item is non-null.
#####################################################################
.namespace ['Wrapped']

#Initializing a Wrapped object directly always fails.
.sub 'init' :vtable
  self.'err'()
.end
.sub 'init_pmc' :vtable
  self.'err'()
.end
.sub 'err' :method
  $S0 = typeof self
  unless $S0 == 'Wrapped' goto skip

  $P0 = new 'Exception'
  $P0 = "Can't construct Wrapped objects directly."
  throw $P0
skip:
.end

#Set the pointed-to object.
.sub 'set_ptr' :method
  .param pmc ptr
  setattribute self, 'ptr', ptr
.end

#Retrieve the pointed-to object.
# Note: This is only necessary within function calls across 
#       the library's boundary.
.sub 'get_ptr' :method
  .local pmc ptr
  ptr = getattribute self, 'ptr'
  .return(ptr)
.end

#Destroy this object; clean up memory if non-null ptr
.sub 'destroy' :vtable
  .local pmc ptr
  ptr = getattribute self, 'ptr'
  unless ptr goto done
  self.'cleanup'(ptr)
  say "Pointer reclaimed"
done:
.end

#Called on destruction; ptr will be non-null
.sub 'cleanup' :method
  .param pmc ptr
.end

#Our Wrapped items can also be Game Objects... we may need a clearer inheritance trail later.
.sub 'update' :method 
  .param num elapsed
.end

#Register this class
.sub Wrapped_class_init :anon :load :init
  $P0 = newclass 'Wrapped'
  addattribute $P0, 'ptr'
.end


