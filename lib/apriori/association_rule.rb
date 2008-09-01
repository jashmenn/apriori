module Apriori

  #  contains two itemsets, "antecendent" and "consequent"
  class AssociationRule
    attr_accessor :antecedent
    attr_accessor :num_antecedent_transactions
    attr_accessor :support

    attr_accessor :consequent
    attr_accessor :confidence

    class << self
      # Given +filename+ of a file containing itemset information returns an
      # Array of +Itemset+s. File format must match that of #parse_line. 
      def from_file(filename)
        rules = []
        contents = File.read(filename)
        contents.each_line do |line|
          rules << parse_line(line)
        end
        rules
      end

      # Given +line+ returns an Itemset 
      # Example of a line:
      #   foo <- bar baz bangle (66.7/4, 75.0)
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

    def to_s
      "%s <- %s (%0.01f%s, %0.01f)" % [ consequent, 
        antecedent.join(" "), 
        support, 
        num_antecedent_transactions ? "/#{num_antecedent_transactions}" : "", confidence ]
    end

  end
end
