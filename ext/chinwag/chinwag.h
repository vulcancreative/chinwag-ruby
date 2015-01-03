//
//  chinwag.h
//  Chinwag
//
//  Created by Chris Calo on 8/05/14.
//  Copyright (c) 2014 Vulcan Creative, LLC. All rights reserved.
//

#ifndef __CHINWAG_AUGXMJP4_H
#define __CHINWAG_AUGXMJP4_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>

// output types
enum {
  CW_LETTERS      =   0,
  CW_WORDS        =   1,
  CW_SENTENCES    =   2,
  CW_PARAGRAPHS   =   3,
};
typedef unsigned long cw_t;

// internal dictionary row
typedef struct dictionary_type {
  bool sorted;
  unsigned long count;
  unsigned long largest;
  unsigned long largest_pos;
  char** words;
} cwdrow_t;

// dictionary (row container)
typedef struct dictionary_container_type {
  bool sorted;
  unsigned long count;
  cwdrow_t* drows;
  char* name;
} cwdict_t;

#include "seuss.h"
#include "latin.h"

#include "ingredient.h"
#include "generator.h"
#include "tokenize.h"
#include "utility.h"
#include "config.h"
#include "dict.h"

char* chinwag
(cw_t type, unsigned long min, unsigned long max, cwdict_t dict);

char* cw_ltr_rng
(unsigned long min, unsigned long max, cwdict_t dict);

char* cw_wrd_rng
(unsigned long min, unsigned long max, cwdict_t dict);

char* cw_snt_rng
(unsigned long min, unsigned long max, cwdict_t dict);

char* cw_pgf_rng
(unsigned long min, unsigned long max, cwdict_t dict);

#define cw_ltr(amt, dict) cw_ltr_rng(amt, amt, dict)
#define cw_wrd(amt, dict) cw_wrd_rng(amt, amt, dict)
#define cw_snt(amt, dict) cw_snt_rng(amt, amt, dict)
#define cw_pgf(amt, dict) cw_pgf_rng(amt, amt, dict)

#endif
