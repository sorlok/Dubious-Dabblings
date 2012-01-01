#####################################################################
# Image class; wraps an sfml::Image* and provides cleanup on destruction
#####################################################################
.namespace ['Image']

#Initialization code.
.sub 'init' :vtable
.end
.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create an Image with args."
  throw $P0
.end

.sub 'load_file' :method
  .param string filename
  .local pmc bb, ptr

  #Convert the string to a byte buffer
  bb = new ['ByteBuffer']
  bb = filename
  push bb, 0

  #Dispatch
  null $P0
  ptr = LIB_dispatch_method($P0, 'new_image', 'pp', bb)
  unless null ptr goto store

  #Error
  $P0 = new 'Exception'
  $P0 = "Image file doesn't exist."
  throw $P0

store:
  #Save it
  $P0 = find_method self, 'set_ptr'
  self.$P0(ptr)
.end

#Reclaim this image.
.sub 'cleanup' :method
  .param pmc ptr
  LIB_delete_image(ptr)
.end

#Retrieve this image's width
.sub 'get_width' :method
  $I0 = LIB_dispatch_method(self, 'image_get_width', 'ip')
  .return($I0)
.end

#Retrieve this image's height
.sub 'get_height' :method
  $I0 = LIB_dispatch_method(self, 'image_get_height', 'ip')
  .return($I0)
.end

#Turn smoothing on or off
.sub 'set_smooth' :method
  .param int smooth
  LIB_dispatch_method(self, 'image_set_smooth', 'vpi', smooth)
.end

#Initialize this class.
.sub Image_class_init :anon :load :init
  $P0 = subclass 'Wrapped', 'Image'
.end


