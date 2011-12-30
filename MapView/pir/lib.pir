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
#TODO: This is pretty bad; can't we dispatch better?
#      Possibly using: http://parrot.github.com/html/docs/compiler_faq.pod.html
#      Lists "optional" and using "flat" syntax. Hopefully that works here.
#TODO: We also need a way to handle return values... can we ".return(void)"?
.sub 'LIB_dispatch_method'
  .param pmc caller
  .param string meth_name
  .param string meth_sig
  .param pmc     arg1  :optional
  .param int has_arg1  :opt_flag
  .param pmc     arg2  :optional
  .param int has_arg2  :opt_flag
  .local pmc lib, func, ptr, args

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
  push args, arg1

  #Add arg2
  unless has_arg2 goto call  
  push args, arg2

call:
  #Retrieve the library/function call
  lib = LIB_get_dll()
  func = dlfunc lib, meth_name, meth_sig
  $P0 = func(args :flat)
  .return($P0)
.end



