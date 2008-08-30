$:.unshift File.join(File.dirname(__FILE__), "..", "lib")
require 'apriori'; include Apriori; puts test_converting_array(["apriori", "test/fixtures/sample.txt", "test/fixtures/results.txt"])
puts
puts
puts
# puts test_hash_ap("one", "two", :hello => "world")
# puts do_apriori
#puts find_association_rules("one", "two", :hello => "world")

# ruby -r 'Apriori/apriori' -e 'include Apriori; puts test_converting_array(["apriori", "test/sample.txt", "test/results.txt"])'
# puts test1
# => 10
# pp help
