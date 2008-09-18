module Apriori

  # This class represents a single association rule.
  # 
  # From Christian's original documentation: 
  # 
  # An association rule is a rule like "If a customer buys wine and bread, he often
  # buys cheese, too."
  # 
  # An association rule states that if we pick a customer at random and find out
  # that he selected certain items (bought certain products, chose certain options
  # etc.), we can be confident, quantified by a percentage, that he also selected
  # certain other items (bought certain other products, chose certain other options
  # etc.).
  # 
  class AssociationRule
    attr_accessor :antecedent
    attr_accessor :num_antecedent_transactions
    attr_accessor :support

    attr_accessor :consequent
    attr_accessor :confidence

    class << self
      # Given +filename+ of a file containing itemset information returns an
      # Array of <tt>Itemset</tt>s. File format must match that of #parse_line. 
      def from_file(filename)
        rules = []
        begin
        contents = File.read(filename)
        contents.each_line do |line|
          rules << parse_line(line)
        end
        rescue => e
          puts "Error reading: #{filename}"
          puts e
        end
        rules
      end

      # Given +line+ returns an Itemset 
      # Example of a line:
      #   foo <- bar baz bangle (66.7/4, 75.0)
      # 
      # Note that this is the opposite order of how apriori.rb returns the
      # AssociationRule#to_s.  (apriori.rb returns the antecedent on the left
      # and consequent on the right)
      def parse_line(line)
        is = new
        line =~ /(.+)\s+<-\s+(.+?)\s+\((\d+\.\d)(?:\/(\d+))?,\s+(\d+\.\d)\)/
        consequent, antecedent, support, transactions, confidence = $1, $2, $3, $4, $5
        is.consequent = consequent 
        is.antecedent = antecedent.split(/\s+/)
        is.support = support.to_f
        is.num_antecedent_transactions = transactions ? transactions.to_i : nil
        is.confidence = confidence.to_f
        is
      end
    end

    # Returns the standard form of this rule as a string. For instance:
    #   bar baz bangle -> foo (66.7/4, 75.0)
    # # (antecedent)     (consequent)
    #
    # Note that this order is the opposite order of the association rules returned by apriori.c
    # I believe this format reads more naturally.
    def to_s
      # "%s <- %s (%0.01f%s, %0.01f)" % [ consequent, 
      #   antecedent.join(" "), 
      #   support, 
      #   num_antecedent_transactions ? "/#{num_antecedent_transactions}" : "", confidence ]
      "%s -> %s (%0.01f%s, %0.01f)" % [ antecedent.join(" "),
        consequent, 
        support, 
        num_antecedent_transactions ? "/#{num_antecedent_transactions}" : "", confidence ]
    end

    def eql?(object) #:nodoc:
      self == (object)
    end

    # Check equality between to <tt>AssociationRule</tt>s
    def ==(object)
      return true if object.equal?(self)
      if object.instance_of?(self.class)
        %w{antecedent num_antecedent_transactions 
           support consequent confidence}.each do |key|
          return false unless object.send(key) == self.send(key)
        end
        return true
      else
        return false
      end
    end

  end
end
