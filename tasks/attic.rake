# This task is not actually used. 
# Use "ruby setup.rb" instead. 
# This task is just here to show you the idea of what is going on
ext_base = File.join(File.dirname(__FILE__), "ext")
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
