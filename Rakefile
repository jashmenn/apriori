require 'fileutils'

ext_base = File.join(File.dirname(__FILE__), "ext")

desc "Do everything"
# TODO - need to basically rm them all?
task "get_it_done" do
  sh "
  pushd #{ext_base}/util/src/;
      rm *.o
      make;
      popd;
  pushd #{ext_base}/math/src;
      rm *.o
      # dont need to make this
      popd;
  pushd #{ext_base}/apriori/src/;
      rm *.o;
      make;
      popd;
  pushd #{ext_base};
      rm *.o;
      rm *.bundle;
      rm Makefile;
      ruby extconf.rb;
      make;
      popd;
      ruby -d test/apriori_test.rb;
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
