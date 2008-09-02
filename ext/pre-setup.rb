starting_dir = Dir.pwd
Dir.chdir("util/src")
system "make"
Dir.chdir(starting_dir)

starting_dir = Dir.pwd
Dir.chdir("apriori/src")
system "make"
Dir.chdir(starting_dir)
