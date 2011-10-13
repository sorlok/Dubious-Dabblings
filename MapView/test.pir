.namespace ['TestGameObject']
.sub 'init' :vtable
.end

.sub 'update' :method
  .param pmc lib 
  .local pmc func

  #Retrieve mouse x,y
  func = dlfunc lib, "game_get_mouse_x", "i"
  $I0 = func()
  func = dlfunc lib, "game_get_mouse_y", "i"
  $I1 = func()

  #Set polygon's position correctly (proves that Parrot is driving the game)
  func = dlfunc lib, "game_set_poly_pos", "vii"
  func($I0 ,$I1)

.end



#Basic game order:
#init_sfml();         //Returns true/false
#CALL PARROT CODE TO CREATE GAME OBJECTS
#while(appRunning) {  //Parrot will have to hook appRunning somehow.
#  sfml_handle_events();  //Could probably return "false" to mean "exit"
#  my_basic_update();     //Do engine-specific stuff. (Currently does lots more.
#  CALL PARROT CODE TO FIX THAT WEIRD POSITIONAL BUG
#  sfml_display();        //Flushes the drawing buffer.
#}
#close_sfml();  //Closing the DLL also works, but let's be thorough.

.namespace[]
.sub 'main' :main
  #Load the DLL
  .local pmc lib, func
  #lib = loadlib "libsfml_engine"  #NOTE: Parrot doesn't use the CWD, so running it from Eclipse without an absolute path will crash.
  lib = loadlib "/home/sethhetu/dubious/MapView/libsfml_engine.so"
  print "Library: "
  say lib
  if lib goto runloop

  #Something's wrong; our librar is undefined.
  say "Library load error."
#  $S0 = dlerror()  #Not called for some reason, even though the code seems to exist.
#  say $S0
  goto done

  runloop:
    #Call init
    func = dlfunc lib, "init_sfml", "i"
    $I0 = func()
    if $I0==0 goto done

    #Create a sample game object
    $P0 = newclass 'TestGameObject'
    $P1 = new ['TestGameObject']

    say "Starting main loop"

    #Main Loop
    main_loop:
      #Have our game handle expected events
      func = dlfunc lib, "sfml_handle_events", "i"
      $I0 = func()

      #Now update within the game loop
      func = dlfunc lib, "my_basic_update", "v"
      func()

      #Call our sample game object's update method
      $P1.'update'(lib)

      #Display what we've just rendered
      func = dlfunc lib, "sfml_display", "v"
      func()

      #Continue to update
      if $I0==0 goto main_loop

    #When we're done, clean up
    func = dlfunc lib, "close_sfml", "v"
    func()

    done:
      say "Done"
.end


