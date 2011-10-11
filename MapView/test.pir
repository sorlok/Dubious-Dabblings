.namespace ['MyClass']
.sub 'init' :vtable
    $P0 = box 'goodbye'
    setattribute self, 'response', $P0
.end

.sub 'getlhs' :method
    .param string a
    .param string b

    $P0 = getattribute self, 'response'
    .local string myattr
    myattr = $P0

    .local string tmp
    tmp = a . myattr
    .return (tmp)
.end



.namespace[]
.sub 'main' :main
    $P0 = newclass 'MyClass'
    addattribute $P0, 'response'


    $P1 = new ['MyClass']
    .local string in1
    in1 = "You know, " 
    .local string in2
    in2 = "ignored" 
    .local string out1 

    out1= $P1.'getlhs'(in1, in2)
    say out1
.end

