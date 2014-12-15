require 'rake/extensiontask'
require 'rubygems/package_task'
require 'rspec/core/rake_task'

RSpec::Core::RakeTask.new

Rake::ExtensionTask.new "chinwag" do |ext|
  ext.lib_dir = "lib/chinwag"
end

s = Gem::Specification.new "chinwag", "0.1.1" do |s|
  s.authors = ["Chris Calo"]
  s.email = ["ccalo@vulcanca.com"]
  s.summary = "A text-synthesis library, for use in layout testing (and more)."
  s.description = "#{s.summary} It houses flexible methods for generation, and a dictionary class (CWDict) allowing adequate token files (or embedded defaults) to be used as the basis for output creation. It effectively supersedes modern Lorem Ipsum generators, in terms of functionality and speed."
  s.homepage = "https://github.com/vulcancreative/chinwag-ruby"
  s.license = "MIT"

  s.files         = Dir["Rakefile"]
  s.files        += Dir["lib/**/*.rb"]
  s.files        += Dir["ext/**/*.h"]
  s.files        += Dir["ext/**/*.c"]
  s.files        += Dir["ext/chinwag/extconf.rb"]

  s.test_files    = s.files.grep(%r{^(test|spec|features)/})
  s.extensions    = Dir["ext/**/extconf.rb"]
  s.require_paths = ["lib", "ext"]
end

Gem::PackageTask.new s do end

task :spec => [:clean, :clobber, :compile]

task :default => :spec
task :test => :spec
