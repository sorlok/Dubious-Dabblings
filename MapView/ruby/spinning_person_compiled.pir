
.HLL "cardinal"

.namespace []
.sub "_block1000" :load :main :anon :subid("10_1325588189.73568")
    .param pmc param_1007 :optional :named("!BLOCK")
    .param int has_param_1007 :opt_flag
.annotate 'file', "spinning_person.rb"
.annotate 'line', 0
    .const 'Sub' $P1017 = "16_1325588189.73568" 
    capture_lex $P1017
    .const 'Sub' $P1009 = "13_1325588189.73568" 
    capture_lex $P1009
    .const 'Sub' $P1003 = "11_1325588189.73568" 
    capture_lex $P1003
.annotate 'line', 1
    if has_param_1007, optparam_18
    new $P105, "Undef"
    set param_1007, $P105
  optparam_18:
    .lex "!BLOCK", param_1007
.annotate 'line', 9
    .const 'Sub' $P1009 = "13_1325588189.73568" 
    capture_lex $P1009
.annotate 'line', 1
    .return ($P1009)
.end


.HLL "cardinal"

.namespace []
.sub "" :load :init :subid("post17") :outer("10_1325588189.73568")
.annotate 'file', "spinning_person.rb"
.annotate 'line', 0
    .const 'Sub' $P1001 = "10_1325588189.73568" 
    .local pmc block
    set block, $P1001
.end


.HLL "parrot"

.namespace []
.sub "_block1016" :init :load :anon :subid("16_1325588189.73568") :outer("10_1325588189.73568")
.annotate 'file', "spinning_person.rb"
.annotate 'line', 0
$P0 = compreg "cardinal"
unless null $P0 goto have_cardinal
load_bytecode "cardinal.pbc"
have_cardinal:
    .return ()
.end


.HLL "cardinal"

.namespace []
.sub "_block1002" :init :load :anon :subid("11_1325588189.73568") :outer("10_1325588189.73568")
.annotate 'file', "spinning_person.rb"
.annotate 'line', 1
    .const 'Sub' $P1006 = "12_1325588189.73568" 
    capture_lex $P1006
    .lex "$def", $P1004
    $P100 = "!keyword_class"("SpinningPerson")
    store_lex "$def", $P100
    find_lex $P101, "$def"
    "!keyword_has"($P101, "@rotation")
    get_hll_global $P102, ["CardinalObject"], "!CARDINALMETA"
    find_lex $P103, "$def"
    $P104 = $P102."register"($P103, "CardinalObject" :named("parent"))
    .return ($P104)
.end


.HLL "cardinal"

.namespace []
.sub "_block1005"  :anon :subid("12_1325588189.73568") :outer("11_1325588189.73568")
.annotate 'file', "spinning_person.rb"
.annotate 'line', 1
    .return ()
.end


.HLL "cardinal"

.namespace ["SpinningPerson"]
.sub "_block1008" :init :load :subid("13_1325588189.73568") :outer("10_1325588189.73568")
    .param pmc param_1010 :optional :named("!BLOCK")
    .param int has_param_1010 :opt_flag
.annotate 'file', "spinning_person.rb"
.annotate 'line', 9
    .const 'Sub' $P1013 = "15_1325588189.73568" 
    capture_lex $P1013
    .const 'Sub' $P1011 = "14_1325588189.73568" 
    capture_lex $P1011
    if has_param_1010, optparam_19
    new $P106, "Undef"
    set param_1010, $P106
  optparam_19:
    .lex "!BLOCK", param_1010
    .const 'Sub' $P1013 = "15_1325588189.73568" 
    capture_lex $P1013
    .return ($P1013)
.end


.HLL "cardinal"

.namespace ["SpinningPerson"]
.sub "initialize" :method :subid("14_1325588189.73568") :outer("13_1325588189.73568")
    .param pmc param_1012 :optional :named("!BLOCK")
    .param int has_param_1012 :opt_flag
.annotate 'file', "spinning_person.rb"
.annotate 'line', 9
    if has_param_1012, optparam_20
    new $P107, "Undef"
    set param_1012, $P107
  optparam_20:
    .lex "!BLOCK", param_1012
.annotate 'line', 10
    new $P108, "CardinalFloat"
    assign $P108, 0.0
    setattribute self, "@rotation", $P108
.annotate 'line', 9
    .return ($P108)
.end


.HLL "cardinal"

.namespace ["SpinningPerson"]
.sub "update" :method :subid("15_1325588189.73568") :outer("13_1325588189.73568")
    .param pmc param_1014
    .param pmc param_1015 :optional :named("!BLOCK")
    .param int has_param_1015 :opt_flag
.annotate 'file', "spinning_person.rb"
.annotate 'line', 13
    .lex "elapsed", param_1014
    if has_param_1015, optparam_21
    new $P109, "Undef"
    set param_1015, $P109
  optparam_21:
    .lex "!BLOCK", param_1015
.annotate 'line', 15
    new $P110, "CardinalInteger"
    assign $P110, 50
    find_lex $P111, "elapsed"
    unless_null $P111, vivify_22
    new $P111, "Undef"
  vivify_22:
    $P112 = "infix:*"($P110, $P111)
    getattribute $P113, self, "@rotation"
    unless_null $P113, vivify_23
    new $P113, "Undef"
  vivify_23:
    $P114 = "infix:+"($P112, $P113)
    setattribute self, "@rotation", $P114
.annotate 'line', 16
    getattribute $P115, self, "@rotation"
    unless_null $P115, vivify_24
    new $P115, "Undef"
  vivify_24:
    .return($P115)
.annotate 'line', 9
    .return ()
.end

