#--
# Copyright (c) 2008 Nate Murray
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

$: << File.expand_path(File.join(File.dirname(__FILE__), "../ext"))
require 'apriori_ext'
$:.unshift(File.dirname(__FILE__)) unless $:.include?(File.dirname(__FILE__)) || $:.include?(File.expand_path(File.dirname(__FILE__)))

require 'tempfile'
require 'apriori/adapter'
require 'apriori/association_rule'

module Apriori
  # Find association rules. Given +input+ and +opts+ returns an Array of AssociationRules.
  # See README.txt if you are unsure why you would want to do this.
  #
  # +input+ can be an Array of Array's of String objects or a String specifing
  # a path a transactions file.
  # 
  # The options are:
  #
  # * <tt>:min_items</tt>: minimal number of items per rule (default: 1)
  # * <tt>:max_items</tt>: maximal number of items per rule (default: no limit)
  # * <tt>:min_support</tt>: minimal support of a rule (default: 10 (percent %))
  # * <tt>:max_support</tt>: maximal support of a rule (default: 100 (percent %))
  # * <tt>:min_confidence</tt>: minimal confidence of a rule (default: 80 (percent %))
  # * <tt>:output_file</tt>: write the rules to this file instead of returning
  #   AssociationRule objects. If this option is specified the path to this file is returned
  #
  # Examples:
  # 
  # This first example passes in an Array of Arrays of Strings. The idea is
  # that each individual Array of Strings is a transaction and the containing
  # Array is the set of all transactions. 
  #
  # In this example, we call #find_association_rules with the default options.
  #
  #    transactions = [ %w{beer doritos},
  #                     %w{apple cheese},
  #                     %w{apple cheese},
  #                     %w{apple doritos} ]
  #
  #    rules = Apriori.find_association_rules(transactions)
  #
  # In this example we read the transactions from a file. The format of the file
  # is one transaction per line, space separated items. For instance:
  #
  #    # save to /path/to/some/file.txt
  #    beer doritos
  #    apple cheese
  #    apple cheese
  #    apple doritos
  #
  # Here is how to call it, using many options:
  # 
  #    rules = Apriori.find_association_rules("/path/to/some/file.txt",
  #                     :min_items => 2,
  #                     :max_items => 2,
  #                     :min_support => 0.01, 
  #                     :max_support => 100, 
  #                     :min_confidence => 20)
  #
  def self.find_association_rules(input, opts={})
    args = []
    
    # create the input file
    if input.kind_of?(String)
      args << input
    elsif input.kind_of?(Array)
      tempfile = create_temporary_file_from_transactions(input)
      args << tempfile.path
    else
      raise "unknown input"
    end

    # create an output file somewhere
    output_file = nil
    if opts[:output_file]
      output_file = opts[:output_file] 
    else
      tempfile = Tempfile.new("transactions_results_#{$!}_#{rand.to_s}")
      tempfile.close # starts open
      output_file = tempfile.path
    end
    args << output_file

    args << "-m#{opts[:min_items]}"      if opts[:min_items]
    args << "-n#{opts[:max_items]}"      if opts[:max_items]
    args << "-s#{opts[:min_support]}"    if opts[:min_support]
    args << "-S#{opts[:max_support]}"    if opts[:max_support]
    args << "-c#{opts[:min_confidence]}" if opts[:min_confidence]

    args << "-a"

    adapter = Adapter.new
    adapter.call_apriori_with_arguments(args)

    if opts[:output_file]
      return output_file
    else
      return AssociationRule.from_file(output_file)
    end
  end

  private
  def self.create_temporary_file_from_transactions(transactions)
    tempfile = Tempfile.open("transactions_#{$!}_#{rand.to_s}")
    transactions.each do |transaction|
      tempfile.puts transaction.join(" ")
    end
    tempfile.close
    tempfile
  end

end
