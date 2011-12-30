#####################################################################
# Functions to be removed. (Some classes also have DEMO functions)
#####################################################################

.namespace[]

#DEMO: Draw objects from SampleUpdate. To be removed.
.sub 'DEMO_SampleDisplay'
  null $P0
  LIB_dispatch_method($P0, 'demo_display', 'v')
.end

#DEMO: Create a polygon
.sub 'DEMO_InitPoly'
  .local pmc ptr, poly

  #Make it
  null $P0
  ptr = LIB_dispatch_method($P0, 'demo_init_poly', 'p')
  poly = new 'Polygon'

  #Save it
  $P0 = find_method poly, 'set_ptr'
  poly.$P0(ptr)

  .return(poly)
.end

