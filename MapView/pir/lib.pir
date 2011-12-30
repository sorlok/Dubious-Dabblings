#####################################################################
# Various functions related to loading the physical library.
#####################################################################

#Internal function: Get the DLL
.sub 'LIB_get_dll'
  .local pmc lib

  #Already loaded once
  get_hll_global lib, 'library_pmc'
  unless null lib goto end

  #Loading for the first time
  lib = loadlib "/home/sethhetu/dubious/MapView/libsfml_engine.so"
  set_hll_global 'library_pmc', lib
  if lib goto end
  
  #Error
  $P0 = new 'Exception'
  $P0 = "Couldn't find library!"
  throw $P0

  end:
  .return(lib)
.end

#Dispatch a method on _some_ library object. 
.sub 'LIB_dispatch_method'
  .param pmc caller
  .param string meth_name
  .param string meth_sig

  #Program args. Can be primitives (int/string), pointers (pmc), or Wrapped sub-classes.
  # (Wrapped will behave as expected.)
  .param pmc     arg1  :optional
  .param int has_arg1  :opt_flag
  .param pmc     arg2  :optional
  .param int has_arg2  :opt_flag

  #Wrap the result?
  .param string     wrap :optional :named("wrap")
  .param int    has_wrap :opt_flag

  .local pmc lib, func, ptr, args, res, ptr

  #Args
  args = new 'ResizablePMCArray'
  if null caller goto addargs #Skip if caller is null

  #Retrieve the pointer
  $P0 = find_method caller, 'get_ptr'
  ptr = caller.$P0()

  #Build up a list of arguments; start with the pointer
  push args, ptr

addargs:  
  #Add arg1
  unless has_arg1 goto call
  $P1 = arg1
  $I0 = can arg1, 'get_ptr'
  unless $I0 goto addarg1
  $P0 = find_method arg1, 'get_ptr'
  $P1 = arg1.$P0()
addarg1:
  push args, $P1

  #Add arg2
  unless has_arg2 goto call
  $P1 = arg2
  $I0 = can arg2, 'get_ptr'
  unless $I0 goto addarg2
  $P0 = find_method arg2, 'get_ptr'
  $P1 = arg2.$P0()
addarg2:
  push args, $P1

call:
  #Retrieve the library/function call
  lib = LIB_get_dll()
  func = dlfunc lib, meth_name, meth_sig
  ptr = func(args :flat)
  res = ptr
  unless has_wrap goto ret

  #Wrap the result
  res = new wrap
  $P0 = find_method res, 'set_ptr'
  res.$P0(ptr)

ret:
  .return(res)
.end



