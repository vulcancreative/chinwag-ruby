#ifndef __CONFIG_6TCIIEDT_H
#define __CONFIG_6TCIIEDT_H

#include "chinwag.h"

// data used for versioning information
extern const unsigned MAJOR_VERSION;
extern const unsigned MINOR_VERSION;
extern const unsigned PATCH_VERSION;

// data used for compilation information
extern const char* const DATE_YEAR;
extern const char* const DATE_MONTH;
extern const char* const DATE_DAY;
extern const char* const REVISION;

// minimum and maximum fixed buffer sizes
extern const unsigned SMALL_BUFFER;
extern const unsigned LARGE_BUFFER;

// minimum size allowed for dictionary tokens (more == better randomization)
extern const unsigned MIN_DICT_SIZE;

// individual characters used to deliminate dictionary tokens
extern const char* const DELIMITERS;

// default output type
extern const unsigned CW_DEFAULT;

// pre-defined default output caps (both must be unsigned integers)
extern const unsigned DEFAULT_MIN_OUTPUT_LENGTH;
extern const unsigned DEFAULT_MAX_OUTPUT_LENGTH;

// pre-defined caps for determining randomized sentence lengths
extern const unsigned SENTENCE_MIN_WORD_LENGTH;
extern const unsigned SENTENCE_MAX_WORD_LENGTH;

// pre-defined caps for determining randomized paragraph lengths
extern const unsigned PARAGRAPH_MIN_SENTENCE_LENGTH;
extern const unsigned PARAGRAPH_MAX_SENTENCE_LENGTH;

#endif
