#####################################################################
# TileMap class; contains a map made of tiles. (Layers later)
#####################################################################
.namespace ['TileMap']

#Initialization code.
.sub 'init' :vtable
.end
.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create a TileMap with args."
  throw $P0
.end

#Reclaim this game map.
.sub 'cleanup' :method
  .param pmc ptr
  LIB_delete_gamemap(ptr)
.end

#Load a from a file, given the extension.
.sub 'load_file' :method
  .param string filename
.end

#Move this tilemap. The offset specifies where the top-left corner of the map should appear.
.sub 'set_offset' :method
  .param int x
  .param int y
.end

#Retrieve offset: x
.sub 'get_offset_x' :method
.end

#Retrieve offset: y
.sub 'get_offset_y' :method
.end

#Retrieve size of tiles in this map
.sub 'get_tile_size' :method
.end

#Retrieve this map's width in tiles
.sub 'get_width' :method
.end

#Retrieve this map's height in tiles
.sub 'get_height' :method
.end

#Get the tile ID of the tile at (x,y)
.sub 'get_tile_at' :method
  .param int x
  .param int y
.end

#Draw it
.sub 'draw' :method
  .param pmc game
.end

#Initialize this class.
.sub TileMap_class_init :anon :load :init
  $P0 = subclass 'Wrapped','TileMap'
.end


