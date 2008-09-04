task :extension do
  # just compile the extension, dont do a typical setup.rb install
  sh "ruby setup.rb clean"
  sh "ruby setup.rb config"
  sh "ruby setup.rb setup"
end
