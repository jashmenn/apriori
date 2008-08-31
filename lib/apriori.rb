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

module Apriori
  # Find itemsets
  # :min_items      minimal number of items per set/rule/hyperedge (default: 1)
  # :max_items      maximal number of items per set/rule/hyperedge (default: no limit)
  # :min_support    minimal support    of a     set/rule/hyperedge (default: 10%)
  # :max_support    maximal support    of a     set/rule/hyperedge (default: 100%)
  # :min_confidence minimal confidence of a         rule/hyperedge (default: 80%)
  def self.find_itemsets(input, opts={})

    # this will probably become refactored out and then use a block b/c most
    # of the setup is always the same
    adapter = Adapter.new
    args = [input]
    output_file = nil

    # create an output file somewhere
    if opts[:output_file]
      output_file = opts[:output_file] 
    else
      raise "TODO"
    end
    args << output_file

    # :min_items => 1,
    # :max_items => 5,
    # :min_support => 1, 
    # :max_support => 100, 
    # :min_confidence => 20

    args << "-m#{opts[:min_items]}"      if opts[:min_items]
    args << "-n#{opts[:max_items]}"      if opts[:max_items]
    args << "-s#{opts[:min_support]}"    if opts[:min_support]
    args << "-S#{opts[:max_support]}"    if opts[:max_support]
    args << "-c#{opts[:min_confidence]}" if opts[:min_confidence]

    args << "-a"

    adapter.call_apriori_with_arguments(args)
  end

end
