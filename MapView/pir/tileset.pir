#####################################################################
# TileSet class; contains a "tile palette" image and other info.
#####################################################################
.namespace ['TileSet']

#Initialization code.
.sub 'init' :vtable
.end
.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create a TileSet with args."
  throw $P0
.end

#Load a tileset image, with a given filesize.
.sub 'load_file' :method
  .param string filename
  .param int tileSize
.end

#Retrieve the maximum number of tiles in this image.
.sub 'get_size' :method
.end

#Draw the given tile (by id) at the given position.
.sub 'draw_tile' :method
  .param pmc game
  .param int id
  .param int x
  .param int y
.end

#Initialize this class.
.sub TileSet_class_init :anon :load :init
  $P0 = newclass 'TileSet'
.end


