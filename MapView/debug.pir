#Shared library wrappers
.sub 'GetNonClassValue'
  .param pmc lib
  .local pmc func

  func = dlfunc lib, "get_nonclass_value", "f"
  $N0 = func()
  .return($N0)
.end

.sub 'MakeNewClass'
  .param pmc lib
  .local pmc func

  func = dlfunc lib, "make_new_class", "p"
  $P0 = func()

  .return($P0)
.end

.sub 'GetClassValue'
  .param pmc lib
  .param pmc item
  .local pmc func

  func = dlfunc lib, "get_class_value", "fp"
  $N0 = func(item)
  .return($N0)
.end


#Test main function
.sub 'main' :main
  .local pmc lib
  .local pmc obj
  lib = loadlib "libsfml_engine.so"

  #Test the non-class function; this is ok
  $N0 = GetNonClassValue(lib)
  print 'Return value 1 is: '
  say $N0

  #Test the class; here's where it fails
  obj = MakeNewClass(lib)
  $N0 = GetClassValue(lib, obj)

  print 'Return value 2 is: '
  say $N0
.end


