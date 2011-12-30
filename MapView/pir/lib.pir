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
  .param pmc arg1
  .local pmc lib, func, ptr

  #Retrieve the pointer
  $P0 = find_method caller, 'get_ptr'
  ptr = caller.$P0()

  #Retrieve the library/function call
  lib = LIB_get_dll()
  func = dlfunc lib, meth_name, meth_sig
  func(ptr, arg1)
.end


