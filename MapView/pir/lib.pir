#####################################################################
# Various functions related to loading the physical library.
#####################################################################

#Internal function: Get the DLL
.sub 'INT_GetDLL'
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

