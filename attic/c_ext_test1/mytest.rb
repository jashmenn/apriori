# Load in the extension (on OS X this loads ./MyTest/mytest.bundle - unsure about Linux, possibly mytest.so)
require 'MyTest/mytest'

# MyTest is now a module, so we need to include it
include MyTest

# Call and print the result from the test1 method
puts test1

# => 10