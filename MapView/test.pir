##File containing our implementation.
.include 'testp.pir'

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



