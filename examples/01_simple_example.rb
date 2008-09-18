$:.unshift(File.dirname(__FILE__) + "/../lib")
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

# Results: 
# beer -> doritos (33.3/2, 100.0)
# doritos -> beer (50.0/3, 66.7)
# doritos -> apple (50.0/3, 33.3)
# apple -> doritos (66.7/4, 25.0)
# cheese -> apple (50.0/3, 100.0)
# apple -> cheese (66.7/4, 75.0)

# NOTE:
# beer -> doritos (33.3/2, 100.0)
# means: 
# * beer appears in 33.3% (2 total) of the transactions (the support)
# * beer implies doritos 100% of the time (the confidence)
