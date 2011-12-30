##File containing our implementation.
.include 'testp.pir'

#####################################################################
# Sample rendition subclass that loads our existing demo and displays it.
#####################################################################

.namespace ['DemoRendition']
.sub 'init' :vtable
  .local pmc personImg
  .local int w, h

  #Initialize our polyScale property
  $P0 = new 'Float'
  $N0 = 1.0
  $P0 = $N0
  setattribute self, 'polyScale', $P0

  #Initialize our polyScaleDec property
  $P0 = new 'Integer'
  $I0 = 1
  $P0 = $I0
  setattribute self, 'polyScaleDec', $P0

  #Initialize our drawables array
  $P0 = new 'ResizablePMCArray'
  setattribute self, 'drawables', $P0

  #Now, start assigning objects to that array.
  $I0 = PFX_CanUse()
  unless $I0 goto makeobjs

  #Load the shader
  $P1 = PFX_MakeNew('colorize.sfx')
  if null $P1 goto makeobjs

  #Set its properties (for now just use the defaults)
  DEMO_SetDefaultPFX($P1)

  #Save it in our array of objects
  $P0.'push'($P1)
  setattribute self, 'pfxShader', $P1
  
  #Make the rest of our objects
  makeobjs:

  #Images
  personImg = new 'Image'
  personImg.'load_file'('person.png')

  #Retrieve the image's width/height, for later
  w = personImg.'get_width'()
  w /= 2
  h = personImg.'get_height'()
  h /= 2

  #Make sprite 1, set its properties, save it.
  $P3 = new 'Sprite'
  $P3.'set_image'(personImg)
  $I0 = 800/3
  $I1 = 600/2
  $P3.'set_position'($I0, $I1)
  $P3.'set_center'(w, h)
  $P0.'push'($P3)
  setattribute self, 'spr1', $P3


  #Make sprite 2, set its properties, save it.
  $P2 = new 'Sprite'
  $P2.'set_image'(personImg)
  $I0 = 2*800
  $I0 /= 3
  $I1 = 600/2
  $P2.'set_position'($I0, $I1)
  $P2.'set_center'(w, h)
  $P0.'push'($P2)
  setattribute self, 'spr2', $P2

  #Make our cursor sprite
  $P1 = DEMO_InitPoly()
  $P0.'push'($P1)
  setattribute self, 'polygon', $P1


.end

.sub 'flip_poly_point' :method
  .param int id1
  .param int id2
  .local pmc poly
  poly = getattribute self, 'polygon'
  $P0 = poly.'get_point_color'(id1)
  $P1 = poly.'get_point_color'(id2)
  poly.'set_point_color'(id1, $P1)
  poly.'set_point_color'(id2, $P0)
.end


.sub 'update' :method 
  .local pmc drawables 
  .local int index, size, polyScaleDec, mouseX, mouseY
  .local num frameTimeS, polyScale

  #Retrieve game timer, current scale
  frameTimeS = GAME_GetFrameTimeInS()
  $P0 = getattribute self, 'polyScale'
  polyScale = $P0
  $P0 = getattribute self, 'polyScaleDec'
  polyScaleDec = $P0

  #Set input manager?
  $P0 = getattribute self, 'input'
  unless null $P0 goto getinput
  $P0 = new ['Input']
  setattribute self, 'input', $P0

getinput:
  #Retrieve mouse x,y
  mouseX = $P0.'get_mouse_x'()
  mouseY = $P0.'get_mouse_y'()

  #Update our post-effect shader
  $P0 = getattribute self, 'pfxShader'   #Note: This _could_ be null...
  DEMO_UpdatePFXColor($P0)

  #Update sprite 1's rotation
  $P1 = getattribute self, 'spr1'
  $N0 = 50 * frameTimeS
  $N1 = $P1.'get_rotation'()
  $N0 += $N1
  $P1.'set_rotation'($N0)

  #Update sprite 1's colorization
  $P0 = $P1.'get_color'()
  $N0 = 1.0-polyScale
  $N0 *= 255
  $I0 = $N0
  $P0.'set_blue'($I0)
  $P1.'set_color'($P0)
  #GAME_DeleteColor($P0) #Should be automatic

  #Update sprite 2's rotation
  $P1 = getattribute self, 'spr2'
  $N0 = 80 * frameTimeS
  $N1 = $P1.'get_rotation'()
  $N0 = $N1 - $N0
  $P1.'set_rotation'($N0)

  #Update sprite 2's colorization
  $P0 = $P1.'get_color'($P1)
  $N0 = polyScale
  $N0 *= 255
  $I0 = $N0
  $P0.'set_alpha'($I0)
  $P1.'set_color'($P0)
  #GAME_DeleteColor($P0)

  #Update our polygon's scale factor
  $N0 = 1
  unless polyScaleDec==1 goto polyscaleupdate
  $N0 = -1

polyscaleupdate:
  $N0 *= frameTimeS
  polyScale += $N0

  #Boundary conditions
  if polyScale < 0.0 goto ltzero
  if polyScale > 1.0 goto gtone
  goto savepoly

ltzero:
  polyScale = 0.0
  polyScaleDec = 0
  self.'flip_poly_point'(1, 5)
  self.'flip_poly_point'(2, 4)
  goto savepolydec

gtone:
  polyScale = 1.0
  polyScaleDec = 1

savepolydec:
  $P0 = getattribute self, 'polyScaleDec'
  $P0 = polyScaleDec
  setattribute self, 'polyScaleDec', $P0

savepoly:
  $P0 = getattribute self, 'polyScale'
  $P0 = polyScale
  setattribute self, 'polyScale', $P0

  #Set the polyline's x-scale
  $P0 = getattribute self, 'polygon'
  $P0.'set_scale_x'(polyScale)

  #Set the polyline's position
  $P0.'set_pos_x'(mouseX)
  $P0.'set_pos_y'(mouseY)
.end

.sub 'display' :method
  .local pmc drawables 
  .local int index, size

  #Now, update any objects in our 'drawables' 
  drawables = getattribute self, 'drawables'
  index = drawables

  at_item:
    index -= 1
    if index < 0 goto done
    $P0 = drawables[index]
    GAME_DrawItem($P0)
    goto at_item
  done:
.end


.sub DemoRendition_class_init :anon :load :init
    #Set up inheritance
    $P2 = subclass 'Rendition', 'DemoRendition'

    #Assign some attributes
    addattribute $P2, 'drawables'
    addattribute $P2, 'pfxShader'
    addattribute $P2, 'spr1'
    addattribute $P2, 'spr2'
    addattribute $P2, 'polyScale'
    addattribute $P2, 'polyScaleDec'
    addattribute $P2, 'polygon'
    addattribute $P2, 'input'
.end


#####################################################################
# Main function
##################################################################### 

.namespace[]
.sub 'main' :main
    .local pmc currRend

    #Create an object of the subclass.
    currRend = new ['DemoRendition']

    #Run it.
    run_game(currRend)
.end



