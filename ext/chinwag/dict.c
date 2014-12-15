#include "dict.h"

drow_t open_drow()
{
  drow_t d;

  // set default values
  d.sorted = false;
  d.count = 0;
  d.largest = 0;
  d.largest_pos = 0;
  d.words = NULL;

  return d;
}

drow_t add_word_to_drow(drow_t drow, const char* word)
{
  // cache strlen of word
  U32 len = strlen(word);

  if(len > 0)
  {
    // increment word count
    ++drow.count;

    // increase dictionary size
    drow.words = (char**)realloc(drow.words, sizeof(char*) * drow.count);

    // add space for new word at added location; copy word
    drow.words[drow.count - 1] = (char*)malloc(len + 1);
    strcpy(drow.words[drow.count - 1], word);
    drow.words[drow.count - 1][len] = '\0';

    // set new largest (if applicable)
    if(drow.largest < len)
    {
      drow.largest = len;
      drow.largest_pos = drow.count - 1;
    }
  }

  return drow;
}

drow_t add_word_to_drow_limit(drow_t drow, const char* word, U32 size)
{
  if(strlen(word) == size) drow = add_word_to_drow(drow, word);
  return drow;
}

drow_t bubble_drow(drow_t drow)
{
  for(U32 i = 0; i != drow.count - 1; ++i)
  {
    if(drow_word_blank(drow, i) && drow_word_present(drow, i + 1))
    {
      // bubble next one into current one
      U32 len = strlen(drow.words[i + 1]);

      // fprintf(stdout, "drow.words[i] : %s\n", drow.words[i]);
      if(drow.words[i] != NULL) free(drow.words[i]);
      drow.words[i] = (char*)malloc(len + 1);
      strcpy(drow.words[i], drow.words[i + 1]);
      drow.words[i][len] = '\0';

      // clear next one that we just copied
      free(drow.words[i + 1]);
      drow.words[i + 1] = NULL;
    }
  }

  drow.sorted = true;

  return drow;
}

bool drow_word_blank(drow_t drow, U32 i)
{
  if(drow.words[i] == NULL || strlen(drow.words[i]) < 1)
  {
    return true;
  }

  return false;
}

bool drow_word_present(drow_t drow, U32 i)
{
  if(drow.words[i] != NULL && strlen(drow.words[i]) > 0)
  {
    return true;
  }

  return false;
}

bool drow_exclude(drow_t drow, char const* str)
{
  for(U32 i = 0; i != drow.count; ++i)
  {
    if(strcmp(drow.words[i], str) == 0) return false;
  }

  return true;
}

bool drow_include(drow_t drow, char const* str)
{
  for(U32 i = 0; i != drow.count; ++i)
  {
    if(strcmp(drow.words[i], str) == 0) return true;
  }

  return false;
}

char* sample_drow(drow_t drow)
{
  // immediately fail if empty
  if(drow.count == 0) return NULL;

  U32 max = (drow.count == 1 ? 0 : drow.count - 1);
  U32 internal = (max == 0 ? 0 : motherr(0, max));

  return drow.words[internal];
}

void puts_drow(drow_t drow)
{
  #ifdef DEBUG
  fprintf(stdout, "(%d)", drow.count);
  #endif

  fprintf(stdout, "[");

  for(U32 i = 0; i != drow.count; ++i)
  {
    fprintf(stdout, "\"%s\"", drow.words[i]);
    if(i < drow.count - 1) fprintf(stdout, ",");
  }

  fprintf(stdout, "]");
}

void close_drow(drow_t drow)
{
  for(U32 i = 0; i != drow.count; ++i)
  {
    if(drow.words[i]) free(drow.words[i]);
  }

  if(drow.words) free(drow.words);
}

dict_t open_dict()
{
  dict_t d;

  // set default values
  d.sorted = false;
  d.count = 0;
  d.drows = NULL;
  d.name = NULL;

  return d;
}

dict_t open_dict_with_name(const char* name)
{
  dict_t d;

  // set default values
  d.sorted = false;
  d.count = 0;
  d.drows = NULL;
  d.name = NULL;

  d.name = (char*)malloc((strlen(name) + 1) * sizeof(char));
  strcpy(d.name, name);

  return d;
}

dict_t open_dict_with_tokens(const char* const buffer, const char* delimiters)
{
  dict_t d = tokenize(buffer, delimiters);
  d.name = NULL;

  return d;
}

dict_t open_dict_with_name_and_tokens(const char* name, const char* const buffer, const char* delimiters)
{
  dict_t d = tokenize(buffer, delimiters);
  d.name = NULL;

  d.name = (char*)malloc((strlen(name) + 1) * sizeof(char));
  strcpy(d.name, name);

  return d;
}

dict_t add_drow_to_dict(dict_t dict, drow_t drow)
{
  ++dict.count;

  dict.drows = (drow_t*)realloc(dict.drows, sizeof(drow_t) * dict.count);
  dict.drows[dict.count - 1] = drow;

  return dict;
}

dict_t add_drow_to_dict_strict(dict_t dict, drow_t drow, U32 size)
{
  if(drow.count >= size) dict = add_drow_to_dict(dict, drow);
  return dict;
}


dict_t place_word_in_dict(dict_t dict, const char* word)
{
  drow_t drow = open_drow();

  drow = add_word_to_drow(drow, word);
  dict = add_drow_to_dict(dict, drow);

  return dict;
}

dict_t place_words_in_dict(dict_t dict, const char* const* words, U32 s)
{
  for(U32 i = 0; i != s; ++i)
  {
    dict = place_word_in_dict(dict, words[i]);
  }

  return dict;
}

dict_t place_word_in_dict_strict(dict_t dict, const char* word)
{
  bool inserted = false;
  U32 len = strlen(word);

  for(U32 i = 0; i != dict.count; ++i)
  {
    if(dict.drows[i].largest == len)
    {
      dict.drows[i] = add_word_to_drow(dict.drows[i], word);
      inserted = true;
    }
  }

  if(inserted == false) dict = place_word_in_dict(dict, word);

  return dict;
}

dict_t place_words_in_dict_strict(dict_t dict, const char* const* words, 
U32 s)
{
  for(U32 i = 0; i != s; ++i)
  {
    dict = place_word_in_dict_strict(dict, words[i]);
  }

  return dict;
}

dict_t bubble_dict(dict_t dict)
{
  drow_t temp;

  // sort individual drows' contents
  for(U32 i = 0; i != dict.count; ++i)
  {
    dict.drows[i] = bubble_drow(dict.drows[i]);
  }

  // sort individual drows within dict
  for(U32 i = 0; i != dict.count - 1; ++i)
  {
    for(U32 j = 0; j != dict.count - 1; ++j)
    {
      if(dict.drows[j].largest > dict.drows[j + 1].largest)
      {
        temp = dict.drows[j];
        dict.drows[j] = dict.drows[j + 1];
        dict.drows[j + 1] = temp;
      }
    }
  }

  dict.sorted = true;

  return dict;
}

dict_t prune_dict(dict_t dict, bool sorted)
{
  U32 len = 0, size = 0, null_count = 0;
  char* against = NULL;

  for(U32 i = 0; i != dict.count; ++i)
  {
    for(U32 j = 0; j != dict.drows[i].count; ++j)
    {
      if(dict.drows[i].words[j] == NULL) continue;
      against = dict.drows[i].words[j];

      for(U32 k = 0; k != dict.drows[i].count; ++k)
      {
        if(dict.drows[i].words[k] == NULL) continue;
        if(strcmp(against, dict.drows[i].words[k]) == 0 && k != j)
        {
          free(dict.drows[i].words[k]);
          dict.drows[i].words[k] = NULL;
        }
      }
    }
  }

  // sort drows within dict
  if(sorted) dict = bubble_dict(dict);

  // resize individual drows within dict
  for(U32 i = 0; i != dict.count; ++i)
  {
    null_count = 0;

    for(U32 j = 0; j != dict.drows[i].count; ++j)
    {
      if(dict.drows[i].words[j] == NULL) ++null_count;
    }

    if(null_count > 0)
    {
      len = dict.drows[i].count - null_count;
      size = sizeof(char*) * len;

      dict.drows[i].words = (char**)realloc(dict.drows[i].words, size);
      dict.drows[i].count = len;
    }
  }

  if(dict_any_blanks(dict)) dict = prune_dict(dict, sorted);

  return dict;
}

dict_t map_dict(dict_t dict, char* (*f)(char*))
{
  U32 len = 0;
  char* temp = (char*)malloc(sizeof(char) * SMALL_BUFFER);

  for(U32 i = 0; i != dict.count; ++i)
  {
    for(U32 j = 0; j != dict.drows[i].count; ++j)
    {
      strcpy(temp, (*f)(dict.drows[i].words[j]));
      len = strlen(temp);

      // only resize if necessary
      if(len != strlen(dict.drows[i].words[j]))
      {
        dict.drows[i].words[j]=(char*)realloc(dict.drows[i].words[j], len);
      }

      strcpy(dict.drows[i].words[j], temp);
    }
  }

  free(temp);

  return dict;
}

dict_t deep_copy_dict(dict_t dict)
{
  dict_t new;

  if(dict.name) new = open_dict_with_name(dict.name);
  else new = open_dict();

  for(U32 i = 0; i != dict.count; ++i)
  {
    for(U32 j = 0; j != dict.drows[i].count; ++j)
    {
      new = place_word_in_dict_strict(new, dict.drows[i].words[j]);
    }
  }

  if(dict.name)
  {
    new.name = (char*)malloc(strlen(dict.name) + 1);
    strcpy(new.name, dict.name);
    new.name[strlen(dict.name)] = '\0';
  }

  if(dict.sorted) new = bubble_dict(new);

  return new;
}

bool dict_exclude(dict_t dict, char const* str)
{
  for(U32 i = 0; i != dict.count; ++i)
  {
    for(U32 j = 0; j != dict.drows[i].count; ++j)
    {
      if(strcmp(dict.drows[i].words[j], str) == 0) return false;
    }
  }

  return true;
}

bool dict_include(dict_t dict, char const* str)
{
  for(U32 i = 0; i != dict.count; ++i)
  {
    for(U32 j = 0; j != dict.drows[i].count; ++j)
    {
      if(strcmp(dict.drows[i].words[j], str) == 0) return true;
    }
  }

  return false;
}

bool dict_any_blanks(dict_t dict)
{
  for(U32 i = 0; i != dict.count; ++i)
  {
    for(U32 j = 0; j != dict.drows[i].count; ++j)
    {
      if(drow_word_blank(dict.drows[i], j)) return true;
    }
  }

  return false;
}

bool dict_valid(dict_t dict, char** error)
{
  U32 count = 0;

  for(U32 i = 0; i != dict.count; ++i)
  {
    for(U32 j = 0; j != dict.drows[i].count; ++j)
    {
      // valid if word excludes a space
      if(exclude(dict.drows[i].words[j], " ")) ++count;
    }
  }
  
  if(count < MIN_DICT_SIZE)
  {
    *error = (char*)malloc(SMALL_BUFFER);
    sprintf(*error, "too few acceptable entries (%d of %d)",count,
    MIN_DICT_SIZE);

    return false;
  }
    
  if(dict.sorted == false)
  {
    *error = (char*)malloc(SMALL_BUFFER);
    sprintf(*error, "dictionary couldn't be sorted");

    return false;
  }

  return true;
}

bool dict_equal(dict_t dict, dict_t against)
{
  if(dict.count != against.count) return false;

  for(U32 i = 0; i != dict.count; ++i)
  {
    if(dict.drows[i].count != against.drows[i].count) return false;

    for(U32 j = 0; j != dict.drows[i].count; ++j)
    {
      if(strcmp(dict.drows[i].words[j], against.drows[i].words[j]) != 0)
      return false;
    }
  }

  return true;
}

bool dict_not_equal(dict_t dict, dict_t against)
{
  if(dict.count != against.count) return true;

  for(U32 i = 0; i != dict.count; ++i)
  {
    if(dict.drows[i].count != against.drows[i].count) return true;

    for(U32 j = 0; j != dict.drows[i].count; ++j)
    {
      if(strcmp(dict.drows[i].words[j], against.drows[i].words[j]) != 0)
      return true;
    }
  }

  return false;
}

I32 find_drow_of_size_in_dict(dict_t dict, U32 largest)
{
  // sort dict if necessary
  if(dict.sorted == false) dict = prune_dict(dict, true);

  for(U32 i = 0; i != dict.count; ++i)
  {
    if(dict.drows[i].count == largest) return i;
  }

  return -1;
}

U32 total_dict(dict_t dict)
{
  U32 total = 0;

  for(U32 i = 0; i != dict.count; ++i) total += dict.drows[i].count;

  return total;
}

U32 dict_largest(dict_t dict)
{
  U32 largest = 0;

  for(U32 i = 0; i != dict.count; ++i)
  {
    if(i == 0) largest = dict.drows[i].largest;
    else if(dict.drows[i].largest > largest) largest=dict.drows[i].largest;
  }

  return largest;
}

char* sample_dict(dict_t dict)
{
  // immediately fail if empty
  if(dict.count == 0) return NULL;

  U32 max = (dict.count == 1 ? 0 : dict.count - 1);
  U32 external = (max == 0 ? 0 : motherr(0, max));

  return sample_drow(dict.drows[external]);
}

char* join_dict(dict_t dict, char const* delimiter)
{
  U32 len = 0, del_len = strlen(delimiter);
  char* string = NULL; char* temp = NULL;

  for(U32 i = 0; i != dict.count; ++i)
  {
    for(U32 j = 0; j != dict.drows[i].count; ++j)
    {
      if(dict.drows[i].words[j] == NULL) continue;
      else if(string) len = strlen(string) + strlen(dict.drows[i].words[j]);
      else len = strlen(dict.drows[i].words[j]);

      if(i <= dict.count - 1 && j <= dict.drows[i].count - 1)
      { len += del_len; }

      temp = (char*)malloc(len + 1);

      if(string)
      {
        strcpy(temp, string);
        strcat(temp, dict.drows[i].words[j]);
      }
      else strcpy(temp, dict.drows[i].words[j]);

      if(i < dict.count - 1 && j <= dict.drows[i].count - 1)
      { strcat(temp, delimiter); }

      temp[len] = '\0';

      if(string) free(string);
      string = temp; temp = NULL;
    }
  }

  return string;
}

void puts_dict(dict_t dict)
{
  fprintf(stdout, "[");

  for(U32 i = 0; i != dict.count; ++i)
  {
    puts_drow(dict.drows[i]);

    if(i < dict.count - 1) fprintf(stdout, ",");

    #ifdef SPLIT
    if(i < dict.count - 1) fprintf(stdout, "\n");
    #endif
  }

  fprintf(stdout, "]");
}

dict_t close_dict(dict_t dict)
{
  for(U32 i = 0; i != dict.count; ++i) close_drow(dict.drows[i]);
  if(dict.drows) { free(dict.drows); dict.drows = NULL; }

  dict.count = 0;
  if(dict.name) { free(dict.name); dict.name = NULL; }

  return open_dict();
}

void validate_dict(dict_t dict, char const* function_name)
{
  U32 count = 0;

  for(U32 i = 0; i != dict.count; ++i)
  {
    for(U32 j = 0; j != dict.drows[i].count; ++j)
    {
      // valide if word excludes a space
      if(exclude(dict.drows[i].words[j], " ")) ++count;
    }
  }
  
  if(count < MIN_DICT_SIZE)
  {
    char e[] =
    "Error in function \"%s\" :\n"
    "  valid dictionary (%d) entry count must be greater than %d\n";
    fprintf(stderr, e, function_name, count, MIN_DICT_SIZE);
    exit(EXIT_FAILURE);
  }

  if(dict.sorted == false)
  {
    char e[] =
    "Error in function \"%s\" :\n"
    "  dictionary is not valid, as it couldn't be properly sorted\n";
    fprintf(stderr, e, function_name);
    exit(EXIT_FAILURE);
  }
}

dict_t split(const char* buffer, const char* delimiters)
{
  char* tok;
  char* mutable_buffer = (char*)malloc(strlen(buffer) + 1 * sizeof(char));
  dict_t dict = open_dict();

  // get mutable copy of buffer; a bit slower, but allows for const-ness
  strcpy(mutable_buffer, buffer);
  mutable_buffer[strlen(buffer)] = '\0';

  // natively tokenize input string
  tok = strtok(mutable_buffer, delimiters);
  while(tok != NULL)
  {
    // add word to dict
    dict = place_word_in_dict(dict, tok);

    // get new tok (if any)
    tok = strtok(NULL, delimiters);
  }

  // close mutable buffer
  free(mutable_buffer);

  return dict;
}

#ifdef DICTMAIN
int main(int argc, const char *argv[])
{
  dict_t dict = open_dict();
  // const char * const words[]= { "the", "quick", "brown", "fox", "jumps", 
  // "over", "the", "lazy", "dog", "dawg" };

  dict = place_words_in_dict_strict(dict, argv, argc);
  dict = prune_dict(dict, true);

  #ifdef DEBUG
  fprintf(stdout, "dict.count : %d\n", dict.count);
  #endif

  puts_dict(dict); fprintf(stdout, "\n");

  close_dict(dict);

  return 0;
}
#endif
