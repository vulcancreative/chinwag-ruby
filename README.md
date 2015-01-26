# Chinwag [![Build Status](https://travis-ci.org/vulcancreative/chinwag-ruby.svg?branch=master)](https://travis-ci.org/vulcancreative/chinwag-ruby)

## Introduction

Chinwag, other than being a funny word, is an open-source, developer toolset used for text-synthesis. The goal is to allow for developers and designers (with a little programming experience) to be able to rapidly prototype text-heavy interfaces, but in a fun manner.

It is primarily written in C99 (for the sake of speed and portability), but has many official language bindings, covering C, Ruby, Python, Swift, and Go.

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


```ruby
# EXAMPLE IN
require 'chinwag'
puts Chinwag.version
```

```
# EXAMPLE OUT
1.2.3
```

## Dictionaries

To generate output, you need to open a dictionary object. The dictionary can be blank, pulled from a custom token file, or loaded from one of Chinwag's embedded options -- `Seussian` or `Latin`.

### Opening an Embedded Dictionary

Typically the easiest way to [generate output](#generation) is to simply use one of the library's embedded dictionaries -- either `Seussian` or `Latin`.

These are installed programmatically, and have their own specific method for access. This is advantageous when utilizing multiple dicitonaries and caching to a global is not an option, as IO bottlenecking isn't a factor.

```ruby
# EXAMPLE IN
require 'chinwag'
seuss = Chinwag::CWDict.open "seussian"
latin = Chinwag::CWDict.open "latin"
```

```
# EXAMPLE OUT
seuss: {
	name: "seussian",
	length: 1096,
	named?: true,
	valid?: true,
	sorted? true,
	_: [
		[I, a], [TV, am, an, as, at, be, ...
		[Mordecai Ali Van Allen O'Shea]
	]
}

latin: {
	name: "Latin",
	length: 35664,
	named?: true,
	valid?: true,
	sorted?: true,
	_: [
		[a, b, c, d, e, f, k, l, m, n, o, ...
		semicircumferentia, supersubstantialis, supertriparticular]
	]
}
```

### Opening a Custom Dictionary

Opening a custom dictionary is very similar to opening an embedded dictionary. Typically the only drawback, however, is that it is a little slower, given that there is often some I/O overhead. Custom dictionaries do need to be [checked for errors](#validation-and-errors) and [sorted](#sorting-and-pruning), as well, prior to [generation](#generation).

If you need a valid, custom dictionary to test against, we recommend our [Noise dictionary](http://vulcanca.com/babble/docs/noise.dict). It has several thousand entries, and will have no problem passing any and all internal validation procedures.

```ruby
# EXAMPLE IN
require 'chinwag'
tokens = File.open("noise.dict", 'r')
noise = Chinwag::CWDict.open tokens
# Chinwag will deduce the dictionary name
# from the file basename, in this case, "noise"
```

```
# EXAMPLE OUT
noise: {
	name: "noise",
	length: 18957,
	named?: true,
	valid?: true,
	sorted? true,
	_: [
		[g, s, u, z, l, h, i, a, m, v, o, q, ...
		pzhvbzvnsdozcuxpgldrwylvedosnbbktoyi]
	]
}
```

### Opening a Blank Dictionary

While having a blank dictionary is not particularly useful, you can append to it after the fact, gradually building a functional dictionary. Blank, unnamed dictionaries have no internal heap allocations, when first initialized.

```ruby
# EXAMPLE IN
require 'chinwag'
blank = Chinwag::CWDict.open
```

```
# EXAMPLE OUT
blank: {
	name: "",
	length: 0,
	named?: false,
	valid?: false,
	sorted? false,
	_: []
}
```

### Examining Dictionaries

If there is ever a reason you need to visually debug a dictionary, each of our libraries supports a visualization component. This forces the dictionary instance to spill its guts, via the command-line.

```ruby
# EXAMPLE IN
require 'chinwag'
seuss = Chinwag::CWDict.open "seussian"
puts seuss
```

```
# EXAMPLE OUT
[[I, a], [TV, am, an, as, at, be, ...
[Dibble Dibble Dibble Dribble], [Mordecai Ali Van Allen O'Shea]]
```

### Dictionary Arithmetic

Whether using an embedded dictionary, or something custom, you can concatenate new entries in the form of strings. This is particularly useful if you have a blank dictionary, and gradually want to build upon it by adding in information dynamically.

```ruby
# EXAMPLE IN
require 'chinwag'
ungrouped = Chinwag::CWDict.open
grouped = Chinwag::CWDict.open
ungrouped += %[these are some test words]
grouped << %w[these words will be sorted]
```

```
# EXAMPLE OUT
ungrouped: {
	name: "",
	length: 5,
	named?: false,
	valid?: false,
	sorted?: false,
	_: [
		[these, are, some, test, words]
	]
}

grouped: {
	name: "",
	length: 5,
	named?: false,
	valid?: false,
	sorted?: false,
	_: [
		[these, words], [will], [be], [sorted]
	]
}
```

### Sorting and Pruning

While generation requires a dictionary to be sorted by length, it is also best-practice to prune your dictionary of repeat elements. Cleaning both sorts and prunes.

```ruby
require 'chinwag'
sorted = Chinwag::CWDict.open "Sorted"
pruned = Chinwag::CWDict.open "Pruned"
cleaned = Chinwag::CWDict.open "Cleaned"
sorted += %w[this is a quick test]
pruned += %w[something something another done]
cleaned += %w[first second first second third]
sorted.sort!
# orders by entry length,
# meeting generation requirements
pruned.prune!
# removes duplicates, retains placement
# needs to be sorted before generating
cleaned.clean!
# removes duplicates and sorts,
# meeting generation requirements
```

```
// EXAMPLE OUT
sorted: {
	name: "Sorted",
	length: 5,
	named?: true,
	valid?: false,
	sorted?: true,
	_ : [
		[a], [is], [test, this], [quick]
	]
}

pruned: {
	name: "Pruned",
	length: 3,
	named?: true,
	valid?: false,
	sorted?: true,
	_ : [
		[something], [another], [done]
	]
}

cleaned: {
	name: "Cleaned",
	length: 3,
	named?: true,
	valid?: false,
	sorted?: true,
	_: [
		[first, third], [second]
	]
}
```

### Duplication

As dictionaries are rooted as complex structs in C99, and require a variety of resources to initialize and close, duplication is a slightly complex procedure.

Nevertheless, we allow deep copies, via the library. Duplication will respect any sorting or pruning that has been done previously to the dictionary being copied, and will have a new address in memory.

```ruby
# EXAMPLE IN
require 'chinwag'
seuss = Chinwag::CWDict.open "seussian"
copy = seuss.clone
```

```
# EXAMPLE OUT
```

### In-Place Modification

Occasionally, one needs to make modifications directly to a dictionary instance. We allow for modifying the instance's internal entries directly via Ruby iteration paradigms. This is particularly useful for, say, converting all entries to uppercase.

```ruby
# EXAMPLE IN
require 'chinwag'
caps = Chinwag::CWDict.open "Caps"
caps << %w[these words will be capitalized]
caps.map! { |e| e.upcase! }
```

```
# EXAMPLE OUT
```

### Closing a Dictionary


```ruby
# EXAMPLE IN
require 'chinwag'
blank = seuss.close
# Clears all of seuss' internal, dynamic memory,
# and resets it to a blank dictionary, which
# you are free to capture
```

```
# EXAMPLE OUT
```

## Validation and Errors

Upon loading a foreign dictionary, it is crucial to test its validity, prior to use. This checks that the library will be capable of understanding the dictionary format properly, and, if so, ensures adequate randomization for our synthesis algorithms.

Embedded dictionaries have already been thoroughly tested, and need no further validation. This, in turn, grants the embedded resources an additional speed boost.

```ruby
# EXAMPLE IN
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
```

```
# EXAMPLE OUT
CWDictTooSmall: too few acceptable entries (0 of 300)
```

## Generation


```ruby
# EXAMPLE IN
require 'chinwag'
seuss = Chinwag::CWDict.open "seussian"
output = Chinwag::generate(:words, 10, 20, seuss)
# Generates ten to twenty words in Seussian
```

```
# EXAMPLE OUT
```

## Legal

Chinwag is available under the [MIT License](http://opensource.org/licenses/MIT).<br>
Use, abuse, and please don't bite the hand that feeds you.
