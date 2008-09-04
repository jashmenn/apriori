= apriori

* This project can be found at: http://github.com/jashmenn/apriori/tree/master
* Christian Borgelt's original C code can be found at: http://www.borgelt.net/apriori.html

== DESCRIPTION:

Ruby Apriori is a library to efficiently find item association rules within
large sets of transactions. This library provides a Ruby interface to Christian
Borgelt's C implementation of this algorithm.

From Christian Borgelt's Apriori:http://www.borgelt.net/apriori.html documentation: 

  Association rule induction is a powerful method for so-called market basket
  analysis, which aims at finding regularities in the shopping behavior of
  customers.

  With the induction of association rules one tries to find sets of
  products that are frequently bought together, so that from the presence of
  certain products in a shopping cart one can infer (with a high probability)
  that certain other products are present. 

  An association rule is a rule like "If a customer buys wine and bread, he often
  buys cheese, too."

  An association rule states that if we pick a customer at random and find out
  that he selected certain items (bought certain products, chose certain options
  etc.), we can be confident, quantified by a percentage, that he also selected
  certain other items (bought certain other products, chose certain other options
  etc.).

This ruby library provides a convenient way to use this algorithm from Ruby.

Original Apriori C code by Christian Borgelt. 

== THE ALGORITHM:

This document is not an introduction to the Apriori algorithm. To find out more about Apriori see:

* http://www.borgelt.net/papers/cstat_02.pdf
* http://www.borgelt.net/papers/fimi_03.pdf
* http://www.borgelt.net/apriori.html
* http://en.wikipedia.org/wiki/Apriori_algorithm

== FEATURES:

* Supports easy use from Ruby data types
* Supports large data files

== EXAMPLE USAGE:

  require 'apriori'

  transactions = [  %w{beer doritos},
                    %w{apple cheese}, 
                    %w{beer doritos}, 
                    %w{apple cheese}, 
                    %w{apple cheese}, 
                    %w{apple doritos} ]

  rules = Apriori.find_association_rules(transactions,
                            :min_items => 2,
                            :max_items => 5,
                            :min_support => 1, 
                            :max_support => 100, 
                            :min_confidence => 20)

  puts rules.join("\n")

  # doritos <- beer (33.3/2, 100.0)
  # means: 
  # * beer appears in 33.3% (2 total) of the transactions (the support)
  # * beer implies doritos 100% of the time (the confidence)

See the +examples+ directory for more examples of usage.

== EXAMPLE DATA:

Example data can be found at:

http://fimi.cs.helsinki.fi/data/

== REQUIREMENTS:

This library is compiled using a slightly modified version of Christian
Borgelt's original C implementation of Apriori. The original code can be found
at: http://www.borgelt.net/apriori.html

* +gcc+ or similar compiler
* <tt>ruby.h</tt>

== INSTALL:

* sudo gem install apriori

== LICENSE:

=== Apriori C code

Copyright (c) Christian Borgelt

Modified and under the LGPL license. 
See ext/apriori/doc/copying for details.

=== Ruby Apriori Extension

(The MIT License)

Copyright (c) 2008 Nate Murray

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

=========================================================================
