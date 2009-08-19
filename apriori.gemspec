Gem::Specification.new do |s|
  s.name = %q{apriori}
  s.version = "0.2.3"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Nate Murray"]
  s.date = %q{2009-08-19}
  s.description = %q{Ruby Apriori is a library to efficiently find item association rules within large sets of transactions.}
  s.email = ["nate@natemurray.com"]
  s.extensions = ["Rakefile"]
  s.extra_rdoc_files = ["History.txt", "License.txt", "Manifest.txt", "README.txt", "TODO.txt", "website/index.txt", "test/fixtures/market_basket_results_test.txt", "test/fixtures/market_basket_string_test.txt", "test/fixtures/results.txt", "test/fixtures/sample.txt"]
  s.files = ["History.txt", "License.txt", "Manifest.txt", "README.txt", "Rakefile", "TODO.txt", "attic/c_ext_test1/MyTest/MyTest.c", "attic/c_ext_test1/MyTest/extconf.rb", "attic/c_ext_test1/mytest.rb", "attic/test.c", "config/hoe.rb", "config/requirements.rb", "examples/01_simple_example.rb", "examples/02_small_file_example.rb", "examples/03_large_file_example.rb", "examples/test_data/market_basket_basic_test.dat", "ext/Apriori.c", "ext/Makefile", "ext/apriori/doc/apriori.html", "ext/apriori/doc/arem.gp", "ext/apriori/doc/c_rev.gp", "ext/apriori/doc/chi2.tex", "ext/apriori/doc/copying", "ext/apriori/doc/line.gif", "ext/apriori/doc/uparrow.gif", "ext/apriori/ex/flg2set", "ext/apriori/ex/hdr2set", "ext/apriori/ex/readme", "ext/apriori/ex/row2set", "ext/apriori/ex/rulesort", "ext/apriori/ex/tab2set", "ext/apriori/ex/test.app", "ext/apriori/ex/test.rul", "ext/apriori/ex/test1.rul", "ext/apriori/ex/test1.tab", "ext/apriori/ex/test2.tab", "ext/apriori/ex/test3.tab", "ext/apriori/ex/test4.tab", "ext/apriori/ex/test5.tab", "ext/apriori/ex/tid2set", "ext/apriori/ex/xhdr2set", "ext/apriori/src/apriori.c", "ext/apriori/src/apriori.dsp", "ext/apriori/src/apriori.dsw", "ext/apriori/src/apriori.mak", "ext/apriori/src/istree.c", "ext/apriori/src/istree.h", "ext/apriori/src/makefile", "ext/apriori/src/tract.c", "ext/apriori/src/tract.h", "ext/apriori_wrapper.c", "ext/apriori_wrapper.h", "ext/extconf.rb", "ext/math/doc/copying", "ext/math/src/chi2.c", "ext/math/src/chi2.h", "ext/math/src/choose.c", "ext/math/src/choose.h", "ext/math/src/gamma.c", "ext/math/src/gamma.h", "ext/math/src/intexp.c", "ext/math/src/intexp.h", "ext/math/src/makefile", "ext/math/src/math.mak", "ext/math/src/normal.c", "ext/math/src/normal.h", "ext/math/src/radfn.c", "ext/math/src/radfn.h", "ext/math/src/zeta.c", "ext/math/src/zeta.h", "ext/pre-clean.rb", "ext/pre-setup.rb", "ext/util/doc/copying", "ext/util/src/listops.c", "ext/util/src/listops.h", "ext/util/src/makefile", "ext/util/src/memsys.c", "ext/util/src/memsys.h", "ext/util/src/nstats.c", "ext/util/src/nstats.h", "ext/util/src/params.c", "ext/util/src/params.h", "ext/util/src/parse.c", "ext/util/src/parse.h", "ext/util/src/scan.c", "ext/util/src/scan.h", "ext/util/src/symtab.c", "ext/util/src/symtab.h", "ext/util/src/tabscan.c", "ext/util/src/tabscan.h", "ext/util/src/util.mak", "ext/util/src/vecops.c", "ext/util/src/vecops.h", "lib/apriori.rb", "lib/apriori/adapter.rb", "lib/apriori/association_rule.rb", "lib/apriori/version.rb", "script/console", "script/destroy", "script/generate", "script/txt2html", "setup.rb", "tasks/apriori.rake", "tasks/attic.rake", "tasks/deployment.rake", "tasks/environment.rake", "tasks/website.rake", "tasks/install.rake", "test/apriori_test.rb", "test/fixtures/market_basket_results_test.txt", "test/fixtures/market_basket_string_test.txt", "test/fixtures/results.txt", "test/fixtures/sample.txt", "test/test_helper.rb", "test/unit/test_apriori.rb", "test/unit/test_itemsets_and_parsing.rb", "website/index.html", "website/index.txt", "website/javascripts/rounded_corners_lite.inc.js", "website/stylesheets/screen.css", "website/template.html.erb"]
  s.has_rdoc = true
  s.homepage = %q{http://apriori.rubyforge.org}
  s.post_install_message = %q{}
  s.rdoc_options = ["--main", "README.txt"]
  s.require_paths = ["lib", "ext"]
  s.rubyforge_project = %q{apriori}
  s.rubygems_version = %q{1.3.1}
  s.summary = %q{Ruby Apriori is a library to efficiently find item association rules within large sets of transactions.}
  s.test_files = ["test/unit/test_apriori.rb", "test/unit/test_itemsets_and_parsing.rb", "test/test_helper.rb"]

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 2

    if Gem::Version.new(Gem::RubyGemsVersion) >= Gem::Version.new('1.2.0') then
      s.add_runtime_dependency(%q<rake>, [">= 0.0.0"])
      s.add_runtime_dependency(%q<newgem>, [">= 0.0.0"])
      s.add_runtime_dependency(%q<rubigen>, [">= 0.0.0"])
      s.add_development_dependency(%q<hoe>, [">= 2.3.3"])
    else
      s.add_dependency(%q<rake>, [">= 0.0.0"])
      s.add_dependency(%q<newgem>, [">= 0.0.0"])
      s.add_dependency(%q<rubigen>, [">= 0.0.0"])
      s.add_dependency(%q<hoe>, [">= 2.3.3"])
    end
  else
    s.add_dependency(%q<rake>, [">= 0.0.0"])
    s.add_dependency(%q<newgem>, [">= 0.0.0"])
    s.add_dependency(%q<rubigen>, [">= 0.0.0"])
    s.add_dependency(%q<hoe>, [">= 2.3.3"])
  end
end
