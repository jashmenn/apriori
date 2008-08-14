# Loads mkmf which is used to make makefiles for Ruby extensions
require 'mkmf'

apriori_root = "../../.."

$objs = ["#{apriori_root}/util/src/vecops.o", 
         "#{apriori_root}/util/src/nimap.o",
         "#{apriori_root}/util/src/tabscan.o",  
         "#{apriori_root}/util/src/scform.o",
         "#{apriori_root}/math/src/gamma.o",
         "#{apriori_root}/math/src/chi2.o", 
         "#{apriori_root}/apriori/src/tract.o",
         "#{apriori_root}/apriori/src/istree.o",
         "apriori_wrapper.o",
         "Apriori.o"]

$CPPFLAGS = " -I#{apriori_root}/util/src -I#{apriori_root}/math/src -I#{apriori_root}/apriori/src "

# gcc -I. -I/usr/local/lib/ruby/1.8/i686-darwin8.10.1 -I/usr/local/lib/ruby/1.8/i686-darwin8.10.1 -I.  -fno-common -g -O2 -pipe -fno-common  -c Apriori.c
# gcc -I. -I/usr/local/lib/ruby/1.8/i686-darwin8.10.1 -I/usr/local/lib/ruby/1.8/i686-darwin8.10.1 -I.  -fno-common -g -O2 -pipe -fno-common  -c apriori_wrapper.c

# Give it a name
extension_name = 'apriori'

# The destination
dir_config(extension_name)

# Do the work
create_makefile(extension_name)
