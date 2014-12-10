#include "tokenize.h"

U32 stringify_file(char** buffer, FILE* fp)
{
  // setup
  U64 sz = 0;

  // determine file size and setup file buffer
  fseek(fp, 0L, SEEK_END);
  sz = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  *buffer = (char*)malloc(sizeof(char) * (sz + 1));
  if(*buffer == NULL) return 0;

  // read file into file buffer
  size_t len = fread(*buffer, sizeof(char), sz, fp);
  if (len == 0) fputs("Error reading file", stderr);
  else (*buffer)[++len] = '\0'; // just to be safe

  fseek(fp, 0L, SEEK_SET);

  return sz;
}

dict_t tokenize(const char* const buffer, const char* delimiters)
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
    // add tok to dict
    dict = place_word_in_dict_strict(dict, tok);

    // get new tok (if any)
    tok = strtok(NULL, delimiters);
  }

  // close mutable buffer
  free(mutable_buffer);

  // clean up dict
  dict = prune_dict(dict, true);

  // validate dict
  validate_dict(dict, "tokenize");

  return dict;
}
