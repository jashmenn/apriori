require File.dirname(__FILE__) + '/../test_helper.rb'

class TestItemsetsAndParsingApriori < Test::Unit::TestCase
  include Apriori

  def setup
  end

  def don_test_calling_all_the_options
    input = File.join(FIXTURES_DIR + "/market_basket_string_test.txt")
  end

  def test_parsing_individual_lines
    # doritos <- beer  (33.3/2, 100.0)
    # apple <- doritos  (50.0/3, 33.3)
    # cheese <- apple  (66.7/4, 75.0)
    # foo <- bar baz  (66.7/4, 75.0)
    # foo <- bar baz bangle (66.7/4, 75.0)

    assert is = Itemset.parse_line("doritos <- beer  (33.3/2, 100.0)")
    wanted = {
      :consequent => "doritos",
      :antecedent => ["beer"],
      :support => 33.3,
      :num_antecedent_transactions => 2,
      :confidence => 100.0,
    }
    wanted.each do |key,value|
      assert_equal value, is.send(key), "Expected itemset '#{key}' to be '#{value}'"
    end

    assert is = Itemset.parse_line("apple <- doritos  (50.0/3, 33.3)")
    wanted = {
      :consequent => "apple",
      :antecedent => ["doritos"],
      :support => 50.0,
      :num_antecedent_transactions => 3,
      :confidence => 33.3,
    }
    wanted.each do |key,value|
      assert_equal value, is.send(key), "Expected itemset '#{key}' to be '#{value}'"
    end

    assert is = Itemset.parse_line("foo <- bar baz  (66.7, 75.0)")
    wanted = {
      :consequent => "foo",
      :antecedent => ["bar", "baz"],
      :support => 66.7,
      :num_antecedent_transactions => nil,
      :confidence => 75.0,
    }
    wanted.each do |key,value|
      assert_equal value, is.send(key), "Expected itemset '#{key}' to be '#{value}'"
    end

    # foo <- bar baz bangle (66.7/4, 75.0)
    assert is = Itemset.parse_line("foo <- bar baz bangle (66.7/4, 75.0)")
    wanted = {
      :consequent => "foo",
      :antecedent => ["bar", "baz", "bangle"],
      :support => 66.7,
      :num_antecedent_transactions => 4,
      :confidence => 75.0,
    }
    wanted.each do |key,value|
      assert_equal value, is.send(key), "Expected itemset '#{key}' to be '#{value}'"
    end


  end


end
