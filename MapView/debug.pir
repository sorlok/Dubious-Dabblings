#Shared library wrappers
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
    obj = MakeNewClass(lib)
    $N0 = GetClassValue(lib, obj)

    print 'Return value is: '
    say $N0
.end


