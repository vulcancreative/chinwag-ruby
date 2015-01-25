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


	EXAMPLE IN
	gem install chinwag -v '~> 1.2'

## Versioning

When we make releases to the API, we strive for consistency across all of the various, language-flavors. Meaning -- when we release an update to the core Chinwag API (in C99), we update all sister components. This should guarantee a consistent version release number across all equivalent libraries.

	EXAMPLE IN
	require 'chinwag'
	puts Chinwag.version

	EXAMPLE OUT
	1.2.3

## Dictionaries

To generate output, you need to open a dictionary object. The dictionary can be blank, pulled from a custom token file, or loaded from one of Chinwag's embedded options -- `Seussian` or `Latin`.

### Opening an Embedded Dictionary

Typically the easiest way to [generate output](Generation) is to simply use one of the library's embedded dictionaries -- either `Seussian` or `Latin`.


	EXAMPLE IN
	require 'chinwag'
	seuss = Chinwag::CWDict.open "Seussian"
	latin = Chinwag::CWDict.open "Latin"

	EXAMPLE OUT

### Opening a Custom Dictionary

Opening a custom dictionary is very similar to opening an embedded dictionary. Typically the only drawback, however, is that it is a little slower, given that there is often some I/O overhead. Custom dictionaries do need to be [checked for errors](Errors) and [sorted](SortingAndPruning), as well, prior to [generation](Generation).




### Opening a Blank Dictionary

While having a blank dictionary is not particularly useful, you can append to it after the fact, gradually building a functional dictionary. Blank, unnamed dictionaries have no internal heap allocations, when first initialized.

	EXAMPLE IN
	require 'chinwag'
	blank = Chinwag::Chinwag.open

	EXAMPLE OUT

### Examining Dictionaries

If there is ever a reason you need to visually debug a dictionary, each of our libraries supports a visualization component. This forces the dictionary instance to spill its guts, via the command-line.

	EXAMPLE IN
	require 'chinwag'
	seuss = Chinwag::CWDict.open "Seussian"
	puts seuss

	EXAMPLE OUT

### Dictionary Arithmetic


	EXAMPLE IN
	require 'chinwag'
	blank = Chinwag::Chinwag.open
	blank += "test"
	blank << %w[quick test]

	EXAMPLE OUT

### Sorting and Pruning




### Duplication

As dictionaries are rooted as complex structs in C99, and require a variety of resources to initalize and close, duplication is a slightly complex procedure.

Nevertheless, we allow deep copies, via the library. Duplication will respect any sorting or pruning that has been done previously to the dictionary being copied, and will have a new address in memory.

	EXAMPLE IN
	require 'chinwag'
	seuss = Chinwag::CWDict.open "Seussian"
	copy = seuss.clone

	EXAMPLE OUT

### In-Place Modification

Occasionally, one needs to make modifications directly to a dictionary instance. We allow for modifiying the instance's internal entries directly via Ruby iteration paradigms. This is particularly useful for, say, converting all entries to uppercase.

	EXAMPLE IN
	require 'chinwag'
	caps = Chinwag::CWDict.open "Caps"
	caps << %w[these words will be capitalized]
	caps.map! { |e| e.upcase! }
	
	EXAMPLE OUT

### Closing a Dictionary


	EXAMPLE IN
	require 'chinwag'
	blank = Chinwag::Chinwag.close seuss
	# Clears all of seuss' internal, dynamic memory,
	# and resets it to a blank dictionary, which
	# you are free to capture

	EXAMPLE OUT

## Validation and Errors


	EXAMPLE IN
	require 'chinwag.h'
	blank = Chinwag::CWDict.open "Blank"

	begin
		blank.validate!
	rescue Chinwag::CWErrorDictTooSmall => e
		# The dictionary had too few valid entries
	rescue Chinwag::CWErrorDictUnsortable => e
		# The dictionary couldn't sort entries by length
	rescue => e
		# You received another error, unrelated to the library

	EXAMPLE OUT
	CWDictTooSmall: too few acceptable entries (0 of 300)

## Generation


	EXAMPLE IN
	require 'chinwag'
	seuss = Chinwag::CWDict.open "Seussian"
	output = Chinwag::generate(:words, 10, 20, seuss)
	# Generates ten to twenty words in Seussian

	EXAMPLE OUT

## Legal

Chinwag is available under the [MIT License](http://opensource.org/licenses/MIT). Use, abuse, and please don't bite the hand that feeds you.
