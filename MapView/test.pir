.sub getlhs
    .param string a
    .param string b

    .local string tmp
    tmp = a . "hello"
    .return (tmp)
.end



.sub 'main' :main
    .local string in1
    in1 = "You know, " 
    .local string in2
    in2 = "ignored" 
    .local string out1 
    out1= getlhs(in1, in2)
    say out1
.end

