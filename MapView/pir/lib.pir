#####################################################################
# Various functions related to loading the physical library.
#####################################################################

.namespace[]

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


#Delete a drawable item.
.sub 'LIB_delete_drawable'
  .param pmc item
  null $P0
  LIB_dispatch_method($P0, 'game_del_item', 'vp', item)
.end

#Delete a color
.sub 'LIB_delete_color'
  .param pmc item
  null $P0
  LIB_dispatch_method($P0, 'game_del_color', 'vp', item)
.end

#Delete an image
.sub 'LIB_delete_texture'
  .param pmc item
  null $P0
  LIB_dispatch_method($P0, 'game_del_texture', 'vp', item)
.end

#Delete a game map
.sub 'LIB_delete_gamemap'
  .param pmc item
  null $P0
  LIB_dispatch_method($P0, 'game_del_gamemap', 'vp', item)
.end

#Delete a render texture
.sub 'LIB_delete_rend_texture'
  .param pmc item
  null $P0
  LIB_dispatch_method($P0, 'game_del_rend_texture', 'vp', item)
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
  .param pmc     arg3  :optional
  .param int has_arg3  :opt_flag
  .param pmc     arg4  :optional
  .param int has_arg4  :opt_flag

  #Wrap the result?
  .param string     wrap :optional :named("wrap")
  .param int    has_wrap :opt_flag

  .local pmc lib, func, ptr, args, res, ptr, bb

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
  $S0 = typeof arg1
  if $S0 == 'String' goto bufferarg1 #Wrap strings in a buffer?
  $P1 = arg1
  $I0 = can arg1, 'get_ptr' #Dereference pointers?
  unless $I0 goto addarg1
  $P0 = find_method arg1, 'get_ptr'
  $P1 = arg1.$P0()
  goto addarg1
bufferarg1:
  $S0 = arg1
  bb = new ['ByteBuffer']
  bb = $S0
  push bb, 0
  $P1 = bb
addarg1:
  push args, $P1

  #Add arg2
  unless has_arg2 goto call
  $S0 = typeof arg2
  if $S0 == 'String' goto bufferarg2 #Wrap strings in a buffer?
  $P1 = arg2
  $I0 = can arg2, 'get_ptr' #Dereference pointers?
  unless $I0 goto addarg2
  $P0 = find_method arg2, 'get_ptr'
  $P1 = arg2.$P0()
  goto addarg2
bufferarg2:
  $S0 = arg2
  bb = new ['ByteBuffer']
  bb = $S0
  push bb, 0
  $P1 = bb
addarg2:
  push args, $P1

  #Add arg3
  unless has_arg3 goto call
  $S0 = typeof arg3
  if $S0 == 'String' goto bufferarg3 #Wrap strings in a buffer?
  $P1 = arg3
  $I0 = can arg3, 'get_ptr' #Dereference pointers?
  unless $I0 goto addarg3
  $P0 = find_method arg3, 'get_ptr'
  $P1 = arg3.$P0()
  goto addarg3
bufferarg3:
  $S0 = arg3
  bb = new ['ByteBuffer']
  bb = $S0
  push bb, 0
  $P1 = bb
addarg3:
  push args, $P1

  #Add arg4
  unless has_arg4 goto call
  $S0 = typeof arg4
  if $S0 == 'String' goto bufferarg4 #Wrap strings in a buffer?
  $P1 = arg4
  $I0 = can arg4, 'get_ptr' #Dereference pointers?
  unless $I0 goto addarg4
  $P0 = find_method arg4, 'get_ptr'
  $P1 = arg4.$P0()
  goto addarg4
bufferarg4:
  $S0 = arg4
  bb = new ['ByteBuffer']
  bb = $S0
  push bb, 0
  $P1 = bb
addarg4:
  push args, $P1

call:
  #Retrieve the library/function call
  lib = LIB_get_dll()
  func = dlfunc lib, meth_name, meth_sig
  if func goto reallycall

  #Error
  $S0 = "Couldn't find function: " . meth_name 
  $S0 .= " ("
  $S0 .= meth_sig
  $S0 .= ")"
  $P0 = new 'Exception'
  $P0 = $S0
  throw $P0

reallycall:
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



