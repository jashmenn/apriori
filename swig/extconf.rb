require 'mkmf'

$INCFLAGS ||= ""
$INCFLAGS += "-I../apriori/src "
$INCFLAGS += "-I../math/src "
$INCFLAGS += "-I../util/src "

create_makefile('apriori')
