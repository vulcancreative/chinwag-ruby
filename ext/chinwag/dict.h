#ifndef __DICT_CO6CSUAJ_H
#define __DICT_CO6CSUAJ_H

#include "chinwag.h"

// dictionary row utilities
drow_t open_drow();
drow_t add_word_to_drow(drow_t drow, const char* word);
drow_t add_word_to_drow_limit(drow_t drow, const char* word, U32 size);
drow_t bubble_drow(drow_t drow);
bool drow_word_blank(drow_t drow, U32 i);
bool drow_word_present(drow_t drow, U32 i);
bool drow_exclude(drow_t drow, char const* str);
bool drow_include(drow_t drow, char const* str);
char* sample_drow(drow_t drow);
void puts_drow(drow_t drow);
void close_drow(drow_t drow);

// dictionary utilities
dict_t open_dict();
dict_t open_dict_with_name(const char* name);
dict_t open_dict_with_tokens(const char* const buffer, const char* delimiters);
dict_t open_dict_with_name_and_tokens(const char* name, const char* const buffer, const char* delimiters);
dict_t add_drow_to_dict(dict_t dict, drow_t drow);
dict_t add_drow_to_dict_strict(dict_t dict, drow_t drow, U32 size);
dict_t place_word_in_dict(dict_t dict, const char* word);
dict_t place_words_in_dict(dict_t dict, const char* const* words, U32 s);
dict_t place_word_in_dict_strict(dict_t dict, const char* word);
dict_t place_words_in_dict_strict(dict_t dict, const char* const* words, U32 s);
dict_t bubble_dict(dict_t dict);
dict_t prune_dict(dict_t dict, bool sorted);
dict_t map_dict(dict_t dict, char* (*f)(char*));
dict_t deep_copy_dict(dict_t dict);
bool dict_exclude(dict_t dict, char const* str);
bool dict_include(dict_t dict, char const* str);
bool dict_valid(dict_t dict, char** error);
I32 find_drow_of_size_in_dict(dict_t dict, U32 largest);
U32 total_dict(dict_t dict);
U32 dict_largest(dict_t dict);
char* sample_dict(dict_t dict);
char* join_dict(dict_t dict, char const* delimiter);
void puts_dict(dict_t dict);
dict_t close_dict(dict_t dict);

// secondary utilities
void validate_dict(dict_t dict, char const* function_name);

#endif
