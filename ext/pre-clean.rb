starting_dir = Dir.pwd
Dir.chdir("util/src")
system "make clean"
Dir.chdir(starting_dir)

Dir.chdir("math/src")
system "make clean"
Dir.chdir(starting_dir)
