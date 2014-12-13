# require 'bundler/gem_tasks'
require 'rake/extensiontask'
require 'rubygems/package_task'

Rake::ExtensionTask.new 'chinwag' do |ext|
  ext.lib_dir = 'lib/chinwag'
end

s = Gem::Specification.new 'chinwag', '1.0' do |s|
  s.summary = 'Ruby variant of Chinwag'
  s.authors = ['Chris Calo']

  s.files         = Dir['Rakefile']
  s.files        += Dir['lib/**/*.rb']
  s.files        += Dir['ext/**/*.h']
  s.files        += Dir['ext/**/*.c']
  s.files        += Dir['ext/chinwag/extconf.rb']

  s.test_files    = s.files.grep(%r{^(test|spec|features)/})
  s.extensions    = Dir['ext/**/extconf.rb']
  # s.executables   = s.files.grep(%r{^bin/}) { |f| File.basename(f) }
  s.require_paths = ['lib', 'ext']
end

Gem::PackageTask.new s do end
