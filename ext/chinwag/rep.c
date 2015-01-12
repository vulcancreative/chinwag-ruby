#include "chinwag.h"

I32 main(I32 argc, const char *argv[])
{
  cwdict_t dict;
  FILE* fp = NULL;
  char* buffer = NULL; char* file_buffer = NULL;

  // [0] = count, [1] = state, [2] = cw_t, [3] = min, [4] = max, [5] = dict
  U32* opt = measure_args(argc, argv, &fp);

  if(opt[0] == 0 || opt[1] == 0)
  {
    free(opt);
    cwdict_close(dict);
    return EXIT_FAILURE;
  }
  else if(opt[1] == 1) return EXIT_SUCCESS;

  // file checking and loading (if applicable); otherwise tokenize internal
  if(opt[5] > 1)
  {
    if(fp == NULL)
    {
      fprintf(stderr, "Expected valid dictionary file argument\n"); help();
      cwdict_close(dict);
      return EXIT_FAILURE;
    }
    else
    {
      U32 stringify_result = stringify_file(&file_buffer, fp);
      if(stringify_result == 0)
      {
        fprintf(stderr, "Unable to read dictionary from file\n"); help();
        cwdict_close(dict);
        return EXIT_FAILURE;
      }

      dict = tokenize(file_buffer, CW_DELIMITERS);
      fclose(fp);
    }
  }
  else if(opt[5] == 1) dict = tokenize(dict_latin, CW_DELIMITERS);
  else dict = tokenize(dict_seuss, CW_DELIMITERS);

  validate_cwdict(dict, "main");

  if(opt[6] == 1)
  {
    dict = cwdict_map(dict, capitalize);
    puts_cwdict(dict);
    fprintf(stdout, "\ndict.count : %lu\n", dict.count);
  }
  else
  {
    // char* result = pgf_rng(opt[3], opt[4], dict);
    // char* result = ltr_rng(33333, 33333, dict);
    char* result = chinwag(opt[2], opt[3], opt[4], dict, NULL);

    fprintf(stdout, "%s\n", result);
    free(result);
  }

  free(file_buffer); free(opt); free(buffer);
  cwdict_close(dict);

  return 0;
}
