# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'chinwag/version'

Gem::Specification.new do |s|
  s.name          = "chinwag"
  s.version       = Chinwag::VERSION
  s.authors       = ["Chris Calo"]
  s.email         = ["ccalo@vulcanca.com"]
  s.summary       = %q{Chinwag - whatever}
  s.description   = %q{See above... this is just a quick test}
  s.homepage      = "https://github.com/vulcancreative/chinwag"
  s.license       = "MIT"

  s.files         = `git ls-files -z`.split("\x0")
  s.test_files    = s.files.grep(%r{^(test|spec|features)/})
  s.extensions    = %w[ext/chinwag/extconf.rb]
  s.executables   = s.files.grep(%r{^bin/}) { |f| File.basename(f) }
  s.require_paths = ["lib"]

  s.add_development_dependency "rspec"
  s.add_development_dependency "bundler", "~> 1.7"
  s.add_development_dependency "rake", "~> 10.0"
end
