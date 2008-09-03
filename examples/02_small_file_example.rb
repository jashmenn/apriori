$:.unshift(File.dirname(__FILE__) + "/../lib")
require 'apriori'
require 'benchmark'

# NOTE: the 'accidents.dat' file is not supplied with this gem. To download it, 
# run: rake apriori:get_example_data 
# or see http://fimi.cs.helsinki.fi/data/

puts "This will take a little while. Hold tight..."

rules = nil
results = Benchmark.measure {
rules = Apriori.find_association_rules(File.dirname(__FILE__) + "/test_data/accidents.dat",
                           :min_items => 2,
                           :max_items => 10,
                           :min_support => 50, 
                           :min_confidence => 80)
}

puts "#{rules.size} rules generated"
puts "Benchmarks:"
puts results
