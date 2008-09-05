task :extension do
  # just compile the extension, dont do a typical setup.rb install
  sh "ruby setup.rb clean"
  sh "ruby setup.rb config"
  sh "ruby setup.rb setup"
end

namespace :github do
  desc "Generate the Gemspec for github"
  task :generate_gemspec do
    sh "rake --silent debug_gem > apriori.gemspec"
  end
end
