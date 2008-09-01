require File.dirname(__FILE__) + '/../test_helper.rb'

class TestAssociationRulesAndParsingApriori < Test::Unit::TestCase
  include Apriori

  def setup
  end

  def test_reading_from_a_file
    input = File.join(FIXTURES_DIR + "/market_basket_results_test.txt")
    assert rules = AssociationRule.from_file(input)
    assert_equal 5, rules.size

    assert is = AssociationRule.parse_line("apple <- doritos  (50.0/3, 33.3)")
    assert rules.include?(is)
    assert is = AssociationRule.parse_line("foo <- bar baz bangle (66.7/4, 75.0)")
    assert rules.include?(is)
  end

  def test_parsing_individual_lines
    assert is = AssociationRule.parse_line("doritos <- beer  (33.3/2, 100.0)")
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
    assert_equal "doritos <- beer (33.3/2, 100.0)", is.to_s

    assert is = AssociationRule.parse_line("apple <- doritos  (50.0/3, 33.3)")
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
    assert_equal "apple <- doritos (50.0/3, 33.3)", is.to_s

    assert is = AssociationRule.parse_line("foo <- bar baz  (66.7, 75.0)")
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
    assert_equal "foo <- bar baz (66.7, 75.0)", is.to_s

    # foo <- bar baz bangle (66.7/4, 75.0)
    assert is = AssociationRule.parse_line("foo <- bar baz bangle (66.7/4, 75.0)")
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
    assert_equal "foo <- bar baz bangle (66.7/4, 75.0)", is.to_s
  end

  def test_association_rule_equality
    assert is  = AssociationRule.parse_line("doritos <- beer  (33.3/2, 100.0)")
    assert is2 = AssociationRule.parse_line("doritos <- beer  (33.3/2, 100.0)")
    assert_equal is, is2
  end


end
