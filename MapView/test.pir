#.namespace ['MyClass']
#.sub 'init' :vtable
#    $P0 = box 'goodbye'
#    setattribute self, 'response', $P0
#.end

#.sub 'getlhs' :method
#    .param string a
#    .param string b

#    $P0 = getattribute self, 'response'
#    .local string myattr
#    myattr = $P0

#    .local string tmp
#    tmp = a . myattr
#    .return (tmp)
#.end



.namespace[]
.sub 'main' :main
#    $P0 = newclass 'MyClass'
#    addattribute $P0, 'response'
#    $P1 = new ['MyClass']
#    .local string in1
#    in1 = "You know, " 
#    .local string in2
#    in2 = "ignored" 
#    .local string out1 
#    out1= $P1.'getlhs'(in1, in2)
#    say out1


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


  #Load the DLL
  .local pmc lib, func
  lib = loadlib "libsfml_engine"

  #Call init
  dlfunc func, lib, "init_sfml", "i"
  $I0 = func()
  if $I0==0 goto done
  say "Starting main loop"

  #Main Loop
  main_loop:
    #Have our game handle expected events
    dlfunc func, lib, "sfml_handle_events", "i"
    $I0 = func()

    #Now update within the game loop
    dlfunc func, lib, "my_basic_update", "v"
    func()

    #Display what we've just rendered
    dlfunc func, lib, "sfml_display", "v"
    func()

    #Continue to update
    if $I0==0 goto main_loop

  #When we're done, clean up
  dlfunc func, lib, "close_sfml", "v"
  func()

  done:

.end


