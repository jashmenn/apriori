require 'fileutils'

desc "Do everything"
# TODO - need to basically rm them all?
task "get_it_done" do
  sh "
  pushd ../../apriori/src/;
      #rm *.o;
      make;
      popd;
  pushd ../../util/src/;
      #rm *.o
      #rm tabscan.o; 
      #rm symtab.o;
      make;
      popd;
  pushd Apriori;
      rm *.o;
      rm *.bundle;
      rm Makefile;
      ruby extconf.rb;
      make;
      popd;
      ruby -d apriori_test.rb;
      "
end

# working_dir = File.dirname(__FILE__) + "/Apriori"

# desc "Do everything"
# task :do_all => [:clean] do 
# end

# file "#{working_dir}/Makefile" do 
#   sh "cd #{working_dir} && ruby extconf.rb"
# end

# task :clean do
#   rm_f FileList['#{working_dir/}*.o'], :verbose => true
# end