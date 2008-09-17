require 'fileutils'
include FileUtils

require 'rubygems'

def check_rubygems_version
  min_version = '1.2.0'
  local_version = %x[gem --version].chomp
 
  unless local_version >= min_version
    puts "You need to update the RubyGems utility to #{min_version} using the following"
    puts ""
    puts " sudo gem update --system"
    exit
  end
end
check_rubygems_version

%w[rake hoe newgem rubigen].each do |req_gem|
  begin
    require req_gem
  rescue LoadError
    puts "This Rakefile requires the '#{req_gem}' RubyGem."
    puts "Installation: gem install #{req_gem} -y"
    exit
  end
end

$:.unshift(File.join(File.dirname(__FILE__), %w[.. lib]))
