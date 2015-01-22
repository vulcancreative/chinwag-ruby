#include "args.h"

U32 help(void)
{
  char *buffer; char text[] =
  "Usage: ./lorem [options]\n"
  "  Options:\n"
  "  -d, [--dict]       # Optional dictionary [embedded: seuss|latin, default: seuss]\n"
  "  -f, [--format]     # Output method [letters, words (default), sentences, paragraphs]\n"
  "  -b, [--min]        # Minimum amount of output [default : 1] (NOTE : must be > 1)\n"
  "  -e, [--max]        # Maximum amount of output [default : 5] (NOTE : must be > min)\n"
  "\n" // "  -t, [--test]       # Spits out a simple sanity test\n\n"
  "  -h, [--help]       # Displays this help menu\n"
  "  -p, [--print]      # Print debug output for dictionary (implicit or passed)\n"
  "  -v, [--version]    # Displays the current version (e.g. - MAJOR.MINOR.PATCH)\n\n"
  ""
  "Do note: external dictionaries must be at least 300 \"tokens\" in length, in order to\n"
  "assure a reasonable amount of randomization. Lorem will reject all dictionaries\n"
  "that do not meet this quota. A \"token\" is considered any word that is separated\n"
  "by a delimiter ([\\s|\\n|,|;|:]). Multiple delimiters may be used, however, if none\n"
  "are used, a dictionary will not be valid, as a result of Lorem seeing it as a lone\n"
  "token. At this time, custom delimiters are not accepted via this command line REP\n"
  "interface, however, they can be used via the programming API.\n\n"
  ""
  "There are two dictionaries embedded in the Lorem application, by default - Latin and\n"
  "Seuss. The first one is composed of roughly 30,000 tokens, using archaic Latin words\n"
  "and the second about 1,000, using words pulled from the collective works of Theodore\n"
  "Giesel (Seuss being the default). To expicitly choose an embeded dictionary, simply\n"
  "its name as the \"-d\" file path argument (e.g. - \"seuss\" or \"latin\").\n"
  ;

  buffer = (char*)malloc(CW_LARGE_BUFFER);
  sprintf(buffer, "%s", text);
  fprintf(stdout, "%s", buffer);

  free(buffer); return EXIT_SUCCESS;
}

U32 version(void)
{
  char *buffer;
  char text[] = "Babble::Lorem, %s (%s-%s-%s revision %s)\n";

  buffer = (char*)malloc(CW_SMALL_BUFFER);
  sprintf(buffer, text, CW_VERSION, CW_DATE_YEAR, CW_DATE_MONTH,
  CW_DATE_DAY, CW_REVISION);
  fprintf(stdout, "%s", buffer);

  free(buffer); return EXIT_SUCCESS;
}

U32 *measure_args(I32 argc, const char *argv[], FILE **fp)
{
  // setup
  U32 *opt = (U32*)malloc(sizeof(U32) * 7);
  memset(opt, 0, 6); // flood initial buffer with 0's, just in case

  // set defaults
  opt[0] = 5; // number of options
  opt[1] = 2; // state - 0:exit failed, 1:exit success, >1:continue
  opt[2] = CW_DEFAULT_TYPE; // output type
  opt[3] = CW_DEFAULT_MIN_OUTPUT; // min output
  opt[4] = CW_DEFAULT_MAX_OUTPUT; // max output
  opt[5] = 0; // dict - 0:seuss, 1:latin, >2:external
  opt[6] = 0; // print - 0:do nothing, 1:print then close

  // check for print
  for(U32 i = 1; i != argc; ++i)
    if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--print") == 0)
      { opt[6] = 1; }

  // read breaker options
  for(U32 i = 1; i != (unsigned)argc; ++i)
    if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
      { help(); opt[1] = 1; return opt; }
    else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
      { version(); opt[1] = 1; return opt; }

  // check for uninterpretable arguments
  for(U32 i = 1; i != (unsigned)argc; ++i)
  {
    if(argv[i][0] == '-')
    {
      if(strcmp(argv[i], "-d") != 0 && strcmp(argv[i], "--dict") != 0 &&
         strcmp(argv[i], "-h") != 0 && strcmp(argv[i], "--help") != 0 &&
         strcmp(argv[i], "-v") != 0 && strcmp(argv[i], "--version") != 0 &&
         strcmp(argv[i], "-f") != 0 && strcmp(argv[i], "--format") != 0 &&
         strcmp(argv[i], "-b") != 0 && strcmp(argv[i], "--min") != 0 &&
         strcmp(argv[i], "-e") != 0 && strcmp(argv[i], "--max") != 0 &&
         strcmp(argv[i], "-p") != 0 && strcmp(argv[i], "--print") != 0
      )
      {
        char e[]="ERROR : uninterpretable symbol \"%s\" received\n";
        fprintf(stderr, e, argv[i]); help();
        opt[0] = 0; return opt;
      }
    }
  }

  // read configuration options
  for(U32 i = 1; i != (unsigned)argc; ++i)
  {
    if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dict") == 0)
    {
      if(i < (unsigned)(argc - 1))
      {
        if(strcmp(argv[i + 1], "seuss") == 0 || strcmp(argv[i + 1], "Seuss") == 0) opt[5] = 0;
        else if(strcmp(argv[i + 1], "latin") == 0 || strcmp(argv[i + 1], "Latin") == 0) opt[5] = 1;
        else { *fp = fopen(argv[i + 1], "r"); opt[5] = 2; }
      }
      else
      {
        char e[]="ERROR : dictionary requested, but invalid argument passed; "
        "expected file dictionary file path, but got \"%s\"\n\n";
        fprintf(stderr, e, argv[i + 1]); help();
        opt[0] = 0; return opt;
      }
    }

    if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--format") == 0)
    {
      if(i < (unsigned)(argc - 1))
      {
        if(strcmp(argv[i + 1], "letters") == 0) opt[2] = CW_LETTERS;
        else if(strcmp(argv[i + 1], "words") == 0) opt[2] = CW_WORDS;
        else if(strcmp(argv[i + 1], "sentences") == 0) opt[2] = CW_SENTENCES;
        else if(strcmp(argv[i + 1], "paragraphs") == 0) opt[2] = CW_PARAGRAPHS;
        else
        {
          char e[]="ERROR : format requested, but invalid argument passed; "
          "expected \"words\", \"sentences\", or \"paragraphs\", "
          "but got \"%s\"\n\n";
          fprintf(stderr, e, argv[i + 1]); help();
          opt[0] = 0; return opt;
        }
      }
      else
      {
        char e[]="ERROR : format requested, but no argument was passed; "
        "expected \"words\", \"sentences\", or \"paragraphs\"\n\n";
        fprintf(stderr, "%s", e); help();
        opt[0] = 0; return opt;
      }
    }

    if(strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--min") == 0)
    {
      if(i < (unsigned)(argc - 1))
      {
        I64 min_input = strtol(argv[i + 1], NULL, 10);

        if(min_input <= 0)
        {
          char e[]="ERROR : minimum output requested, but the argument was "
          "0 or lower; expected an integer greater than, or equal to 1\n\n";
          fprintf(stderr, "%s", e); help();
          opt[0] = 0; return opt;
        }

        opt[3] = min_input;
      }
      else
      {
        char e[]="ERROR : minimum output requested, but no argument was "
        "passed; expected integer greater than 0\n\n";
        fprintf(stderr, "%s", e); help();
        opt[0] = 0; return opt;
      }
    }

    if(strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--max") == 0)
    {
      if(i < (unsigned)(argc - 1))
      {
        I64 max_input = strtol(argv[i + 1], NULL, 10);

        if(max_input <= 0)
        {
          char e[]="ERROR : maximum output requested, but the argument was "
          "0 or lower; expected an integer greater than, or equal to 1\n\n";
          fprintf(stderr, "%s", e); help();
          opt[0] = 0; return opt;
        }

        opt[4] = max_input;
      }
      else
      {
        char e[]="ERROR : maximum output requested, but no argument was "
        "passed; expected integer greater than minimum requested output\n\n";
        fprintf(stderr, "%s", e); help();
        opt[0] = 0; return opt;
      }
    }
  }

  if(opt[4] < opt[3])
  {
    char e[]="ERROR : maximum output requested, but the argument was "
    "lower than the min output; expected an integer greater than %d\n\n";
    fprintf(stderr, e, opt[3] - 1); help();
    opt[0] = 0; return opt;
  }

  return opt;
}
