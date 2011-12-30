###Main library code.

#More stuff
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



#####################################################################
# Main game loop. Makes use of shared library code where possible.
#####################################################################

.namespace[]
.sub 'run_game'
  .local int res
  .param pmc currRend
  .local pmc game

  #Start the game
  game = new 'Game'
  res = game.'init'(800, 600, 32)
  if res==0 goto done

  say "Starting main loop"

  #Main Loop
  main_loop:
    #Have our game handle expected events
    $I0 = game.'process_events'()

    #Now update within the game loop
    #DEMO_SampleUpdate()

    #Call our sample game object's update method
    currRend.'update'()

    #Display what we've just rendered
    DEMO_SampleDisplay()

    #And here is where the real updating happens
    currRend.'display'()

    #Here we go
    game.'display'()

    #Continue to update
    if $I0==0 goto main_loop

  #When we're done, clean up
  game.'close'()

  done:
    say "Done"
.end




