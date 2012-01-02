#####################################################################
# OffscreenTexture class; wraps a RenderTexture
#####################################################################
.namespace ['Game']

#Initialization code.
.sub 'init' :vtable
.end
.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create an OffscreenTexture with args."
  throw $P0
.end

.sub 'init_empty_image' :method
  .param int width
  .param int height
  .param pmc bkgrdColor
  .local pmc ptr

  #Dispatch, save
  null $P0
  ptr = LIB_dispatch_method($P0, 'new_render_texture', 'piip', width, height, bkgrdColor)
  $P0 = find_method self, 'set_ptr'
  self.$P0(ptr)
.end

#Reclaim this resource.
.sub 'cleanup' :method
  .param pmc ptr
  LIB_delete_rend_texture(ptr)
.end


#Draw onto this image
.sub 'draw_item' :method
  .param pmc item
  LIB_dispatch_method(self, 'render_target_draw_item', 'vpp', item)
.end


#Initialize this class.
.sub OffscreenTexture_class_init :anon :load :init
  $P0 = subclass 'Wrapped', 'OffscreenTexture'
.end


