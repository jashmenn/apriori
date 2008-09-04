namespace :apriori do
  desc "Download the example data"
  task :get_example_data do
    puts <<-EOF 
Sorry! This isn't implemented in a portable way just yet.
If you're on a *nix system try: 

curl -0 http://fimi.cs.helsinki.fi/data/kosarak.dat > examples/test_data/kosarak.dat
curl -0 http://fimi.cs.helsinki.fi/data/retail.dat > examples/test_data/retail.dat

Patches welcome! (http://github.com/jashmenn/apriori/tree/master)
EOF

      # open("http://www.ruby-lang.org/") {|f|
      #       f.each_line {|line| p line}
      #         }
      # }

  end
end
