module Apriori
  # This module provides the basic adaptations to the c extension.
  class Adapter
    include Apriori

    # call the actual apriori extension
    def call_apriori_with_arguments(args) #:nodoc:
      args.unshift("apriori")
      do_apriori(args)
    end

  end
end
