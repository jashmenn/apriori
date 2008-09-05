require 'config/requirements'
require 'config/hoe' # setup Hoe + all gem configuration

class Rake::Task
  def abandon
    prerequisites.clear
    @actions.clear
  end
end

Dir['tasks/**/*.rake'].each { |rake| load rake }

Rake::Task[:default].abandon
task :default => :extension 

