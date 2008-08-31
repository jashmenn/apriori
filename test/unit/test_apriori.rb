require File.dirname(__FILE__) + '/../test_helper.rb'

class TestApriori < Test::Unit::TestCase

  def setup
  end

  # require 'apriori'; include Apriori; puts do_apriori(["apriori", "test/fixtures/sample.txt", "test/fixtures/results.txt"])
  
  def dont_test_truth
    input = File.join(FIXTURES_DIR + "/market_basket_string_test.txt")
    pp Apriori.find_itemsets(input, :output_file => "results.txt")
  end

  def test_calling_all_the_options
    input = File.join(FIXTURES_DIR + "/market_basket_string_test.txt")
    pp Apriori.find_itemsets(input, 
                              :output_file => "results2.txt",
                              :min_items => 2,
                              :max_items => 5,
                              :min_support => 1, 
                              :max_support => 100, 
                              :min_confidence => 20)
  end

end
