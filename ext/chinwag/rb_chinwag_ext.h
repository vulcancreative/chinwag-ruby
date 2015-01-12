#ifndef __RB_CHINWAG_EXT_NNA6BCFM_H
#define __RB_CHINWAG_EXT_NNA6BCFM_H

#include <ruby.h>

#ifdef HAVE_RUBY_IO_H
#include <ruby/io.h>
#else
#include <rubyio.h>
#endif

#ifdef HAVE_RUBY_INTERN_H
#include <ruby/intern.h>
#else
#include <intern.h>
#endif

#include "chinwag.h"

extern VALUE m_chinwag;
extern VALUE c_cw_dict;

extern VALUE e_invalid_output_type;
extern VALUE e_min_less_than_one;
extern VALUE e_max_less_than_min;
extern VALUE e_max_too_high;
extern VALUE e_dict_too_small;
extern VALUE e_dict_unsortable;
extern VALUE e_dict_unknown;

#endif
