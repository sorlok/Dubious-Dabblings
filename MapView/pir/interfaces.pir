###Pull in all library code
.include "pir/lib.pir"
.include "pir/wrapped.pir"
.include "pir/color.pir"
.include "pir/input.pir"
.include "pir/texture.pir"
.include "pir/offscreen_texture.pir"
.include "pir/polygon.pir"
.include "pir/sprite.pir"
.include "pir/postfx.pir"
.include "pir/game.pir"
.include "pir/demo.pir"
.include "pir/tileset.pir"
.include "pir/tilemap.pir"

#NOTE: This code is generated. It should probably be included through some other mechanism
.include "ruby/spinning_person_compiled.pir"


#####################################################################
# A Rendition is a way of viewing the game world. The Game Engine calls
#   udpate() and display() every time tick.
# An Rendition is sort of like a "mini" engine; e.g., we might have an 
#   RPG Rendition, which itself contains Renditions for each minigame, and
#   perhaps one for the menu.
#####################################################################
.namespace ['Rendition']
.sub 'init' :vtable
.end

.sub 'update' :method 
.end

.sub 'display' :method
.end

.sub Rendition_class_init :anon :load :init
    newclass $P0, [ 'Rendition' ]
.end










