= apriori

Apriori C code by Christian Borgelt. Taken directly from: todo, url
* FIX (url)

== DESCRIPTION:

FIX (describe your package)

== FEATURES/PROBLEMS:

* FIX (list of features or problems)

== SYNOPSIS:

  FIX (code sample of usage)

== EXAMPLE DATA:

http://fimi.cs.helsinki.fi/data/
http://fimi.cs.helsinki.fi/data/kosarak.dat
http://fimi.cs.helsinki.fi/data/retail.dat

== REQUIREMENTS:

* FIX (list of requirements)

== INSTALL:

* FIX (sudo gem install, anything else)

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

= What options to support later:

-t#      target type (default: association rules)
         (s: item sets, c: closed item sets, m: maximal item sets,
          r: association rules, h: association hyperedges)
-k#      item separator for output (default: " ")
-p#      output format for support/confidence (default: "%.1f")
-y       print lift value (confidence divided by prior)
-g       write output in scanable form (quote certain characters)

-q#      sort items w.r.t. their frequency (default: 2)
         (1: ascending, -1: descending, 0: do not sort,
          2: ascending, -2: descending w.r.t. transaction size sum)
-b/f/r#  blank characters, field and record separators
         (default: " \t\r", " \t", "\n")
appfile  file stating item appearances (optional)


= What options to support even later:
-e#      additional evaluation measure (default: none)
-!       print a list of additional evaluation measures
-d#      minimal value of additional evaluation measure (default: 10%)
-v       print value of additional rule evaluation measure
-l       do not load transactions into memory (work on input file)
-u#      filter unused items from transactions (default: 0.1)
         (0: do not filter items w.r.t. usage in sets,
         <0: fraction of removed items for filtering,
         >0: take execution times ratio into account)
-h       do not organize transactions as a prefix tree
-j       use quicksort to sort the transactions (default: heapsort)
-z       minimize memory usage (default: maximize speed)
-C#      comment characters (default: "#")


Don't even know what they do: 
-o       use original definition of the support of a rule (body & head)
-x       extended support output (print both rule support types)

