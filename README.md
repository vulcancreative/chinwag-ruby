[![Build Status](https://travis-ci.org/vulcancreative/chinwag-ruby.svg?branch=master)](https://travis-ci.org/vulcancreative/chinwag-ruby)

## Introduction

Chinwag, other than being a funny word, is an open-source, developer toolset used for text-synthesis. The goal is to allow for developers and designers (with a little programming experience) to be able to rapidly prototype text-heavy interfaces, but in a fun manner.

It is primarily written in C99 (for the sake of speed and portability), but has many offical language bindings, covering C, Ruby, Python, Swift, and Go.

The library universally features:

* Powerful dictionary type with accompanying routines
* Several embedded dictionaries
* Easy custom dictionary creation and use
* Easy output configuration
* C99-based for speed and portability
* Zero dependencies

## Installation


	gem install chinwag -v '~> 1.2'

## Versioning

When we make releases to the API, we strive for consistency across all of the various, language-flavors. Meaning -- when we release an update to the core Chinwag API (in C99), we update all sister components. This should guarantee a consistent version release number across all equivalent libraries.

```Ruby
