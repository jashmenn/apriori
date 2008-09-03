$:.unshift(File.dirname(__FILE__) + "/../lib")
require 'apriori'
require 'benchmark'

rules = nil
results = Benchmark.measure {
rules = Apriori.find_association_rules(File.dirname(__FILE__) + "/test_data/market_basket_basic_test.dat",
                           :min_items => 2,
                           :max_items => 3,
                           :min_support => 0.01, 
                           :min_confidence => 30)
}

puts rules.join("\n")
puts "#{rules.size} rules generated"
puts "Benchmarks:"
puts results
