#####################################################################
# PostFX class; wraps an sfml::PostFX* and provides cleanup on destruction
#####################################################################
.namespace ['PostFX']

#Initialization code.
.sub 'init' :vtable
.end
.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create a PostFX with args."
  throw $P0
.end

#Reclaim this pfx pointer.
.sub 'cleanup' :method
  .param pmc ptr
  GAME_DeleteItem(ptr)
.end

#Load from file
.sub 'load_file' :method
  .param string filename
  .local pmc bb, ptr

  #Convert the string to a byte buffer
  bb = new ['ByteBuffer']
  bb = filename
  push bb, 0

  #Dispatch
  null $P0
  ptr = LIB_dispatch_method($P0, 'new_postfx', 'pp', bb)
  unless null ptr goto store

  #Error
  $P0 = new 'Exception'
  $P0 = "PostFX file doesn't exist."
  throw $P0

store:
  #Save it
  $P0 = find_method self, 'set_ptr'
  self.$P0(ptr)
.end


#Initialize this class.
.sub PostFX_class_init :anon :load :init
  $P0 = subclass 'Wrapped', 'PostFX'
.end


