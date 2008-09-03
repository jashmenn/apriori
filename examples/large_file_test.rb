$:.unshift(File.dirname(__FILE__) + "/../lib")
require 'apriori'

rules = Apriori.find_association_rules("/Users/nathan/Programming/datamining/purchased/data/large.txt",
                           :min_items => 2,
                           :max_items => 2,
                           :min_support => 0.01, 
                           :max_support => 100, 
                           :min_confidence => 20)

puts rules.join("\n")
