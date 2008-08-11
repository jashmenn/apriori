require 'mkmf'

$INCFLAGS ||= ""
$INCFLAGS += " -I../math/src "

create_makefile('apriori')
