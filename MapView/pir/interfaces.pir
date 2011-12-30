###Pull in all library code
.include "pir/lib.pir"
.include "pir/wrapped.pir"
.include "pir/color.pir"
.include "pir/input.pir"
.include "pir/image.pir"
.include "pir/polygon.pir"
.include "pir/sprite.pir"
.include "pir/postfx.pir"
.include "pir/game.pir"
.include "pir/demo.pir"

#####################################################################
# Base classes, to be subclassed by our library.
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






