.sub 'main' :main
    .param pmc all_args
    $S2 = all_args[1]
    $S1 = "["
    $S1 .= $S2
    $S1 .= "]"
    .return ($S1)
.end

