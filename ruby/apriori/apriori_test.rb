require 'Apriori/apriori'; include Apriori; 
puts
puts
puts
# puts test_hash_ap("one", "two", :hello => "world")
# puts do_apriori
puts find_association_rules("one", "two", :hello => "world")

# ruby -r 'Apriori/apriori' -e 'include Apriori; puts do_apriori'
# puts test1
# => 10
# pp help
