require File.dirname(__FILE__) + '/../test_helper.rb'

class TestApriori < Test::Unit::TestCase

  def setup
  end

  # require 'apriori'; include Apriori; puts do_apriori(["apriori", "test/fixtures/sample.txt", "test/fixtures/results.txt"])
  
  def test_truth
    input = File.join(FIXTURES_DIR + "/market_basket_string_test.txt")
    pp Apriori.find_itemsets(input, :output_file => "results.txt", :appearances => true)

  end
end
