require 'apriori'
require 'pp'

# The first return value gets set as expected in first_returnval.
# However, SWIG puts the float in a second return value.

pp Apriori.intexp(2, 2)

#first_returnval,second_returnval = Example.SimpleFunc(afloat)
#puts "Your float is #{first_returnval}, and the return value is: #{second_returnval}"
