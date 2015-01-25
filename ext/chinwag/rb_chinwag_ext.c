#include "rb_chinwag_ext.h"

VALUE m_chinwag;
VALUE c_cwdict;

VALUE e_invalid_output_type;
VALUE e_min_less_than_one;
VALUE e_max_less_than_min;
VALUE e_max_too_high;
VALUE e_dict_too_small;
VALUE e_dict_unsortable;
VALUE e_dict_unknown;

VALUE default_dict;
VALUE default_output_type;
VALUE default_min_output;
VALUE default_max_output;

cw_t get_cw_t_for_symbol(VALUE symbol)
{
  cw_t cw_type = CW_DEFAULT_TYPE;

  Check_Type(symbol, T_SYMBOL);

  VALUE id = rb_to_id(symbol);

  if(strcmp(rb_id2name(id), "letters") == 0) cw_type = CW_LETTERS;
  else if(strcmp(rb_id2name(id), "words") == 0) cw_type = CW_WORDS;
  else if(strcmp(rb_id2name(id), "sentences") == 0)cw_type=CW_SENTENCES;
  else if(strcmp(rb_id2name(id), "paragraphs") == 0)cw_type=CW_PARAGRAPHS;
  else
  {
    rb_raise(rb_eTypeError, "invalid type (expected :letters, :words, "
    ":sentences, or :paragraphs)");
  }

  return cw_type;
}

VALUE m_cw_generate(int argc, VALUE* argv, VALUE obj)
{
  cwdict_t* d;
  cwerror_t e;
  VALUE result;
  cw_t cw_type = get_cw_t_for_symbol(default_output_type);
  long min = NUM2LONG(default_min_output);
  long max = NUM2LONG(default_max_output);
  char* output;

  // raise exception if passed wrong number of arguments
  if(argc > 4)
  {
    rb_raise(rb_eArgError, "wrong number of arguments (0..4)");
  }

  // do stuff with 'em
  if(argc == 0) Data_Get_Struct(default_dict, cwdict_t, d);

  if(argc >= 1) Data_Get_Struct(argv[0], cwdict_t, d);

  if(argc >= 2) cw_type = get_cw_t_for_symbol(argv[1]);

  if(argc >= 3)
  {
    Check_Type(argv[2], T_FIXNUM);
    long temp_a = FIX2LONG(argv[2]);
    min = temp_a; max = temp_a;
  }

  if(argc == 4)
  {
    Check_Type(argv[3], T_FIXNUM);
    max = FIX2LONG(argv[3]);
  }

  /*
  if(max < min)
  {
    rb_raise(rb_eException,"upper threshold must be more than lower "
    "threshold (min : %lu, max : %lu)", min, max);
  }

  if(min < 1 || max > 10000)
  {
    rb_raise(rb_eRangeError,"out of range (1..10000)");
  }
  */

  if(!cwdict_valid(*d, &e))
  {
    char* message = cwerror_string(*d, e);
    if(e == CWERROR_DICT_TOO_SMALL)
    rb_raise(e_dict_too_small, "%s", message);
    else if(e == CWERROR_DICT_UNSORTABLE)
    rb_raise(e_dict_unsortable, "%s", message);
    else
    rb_raise(e_dict_unknown, "%s", message);

    free(message);
  }

  /*
  switch(cw_type)
  {
    case CW_LETTERS:
      output = chinwag(cw_type, min, max, *d);
      break;
    case CW_WORDS:
      output = chinwag(cw_type, min, max, *d);
      break;
    case CW_SENTENCES:
      output = chinwag(cw_type, min, max, *d);
      break;
    case CW_PARAGRAPHS:
      output = chinwag(cw_type, min, max, *d);
      break;
    default:
      rb_raise(rb_eTypeError, "invalid type (expected LETTERS, WORDS, "
      "SENTENCES, or PARAGRAPHS)");
      break;
  }
  */

  /*
  CWERROR_INVALID_OUTPUT_TYPE   =   0,
  CWERROR_MIN_LESS_THAN_ONE     =   1,
  CWERROR_MAX_LESS_THAN_MIN     =   2,
  CWERROR_MAX_TOO_HIGH          =   3,
  */

  output = chinwag(cw_type, min, max, *d, &e);

  if(output) result = rb_str_new2(output);
  else
  {
    char* message = cwerror_string(*d, e);
    if(e == CWERROR_INVALID_OUTPUT_TYPE)
    rb_raise(e_invalid_output_type, "%s", message);
    else if(e == CWERROR_MIN_LESS_THAN_ONE)
    rb_raise(e_min_less_than_one, "%s", message);
    else if(e == CWERROR_MAX_LESS_THAN_MIN)
    rb_raise(e_max_less_than_min, "%s", message);
    else if(e == CWERROR_MAX_TOO_HIGH)
    rb_raise(e_max_too_high, "%s", message);
    else
    rb_raise(e_dict_unknown, "%s", message);

    free(message);
  }

  return result;
}

VALUE c_cwdict_clone(VALUE obj);
VALUE c_cwdict_close(VALUE obj);
VALUE m_set_d_dict(VALUE obj, VALUE new)
{
  VALUE original = c_cwdict_clone(default_dict);

  default_dict = c_cwdict_close(default_dict);
  default_dict = c_cwdict_clone(new);

  return original;
}

VALUE m_set_d_type(VALUE obj, VALUE sym)
{
  VALUE original = default_output_type;

  Check_Type(sym, T_SYMBOL);

  VALUE id = rb_to_id(sym);

  if(strcmp(rb_id2name(id), "letters") == 0) default_output_type = sym;
  else if(strcmp(rb_id2name(id), "words") == 0) default_output_type = sym;
  else if(strcmp(rb_id2name(id), "sentences") == 0)default_output_type=sym;
  else if(strcmp(rb_id2name(id), "paragraphs") == 0)default_output_type=sym;
  else
  {
    rb_raise(rb_eTypeError, "invalid type (expected :letters, :words, "
    ":sentences, or :paragraphs)");
  }

  return original;
}

VALUE m_s_min(VALUE obj, VALUE num)
{
  VALUE original = default_min_output;

  Check_Type(num, T_FIXNUM);

  default_min_output = num;

  long min = NUM2LONG(default_min_output);
  long max = NUM2LONG(default_max_output);

  if(max < min)
  {
    rb_raise(rb_eException,"upper threshold must be more than lower "
    "threshold (min : %lu, max : %lu)", min, max);
  }

  if(min < 1 || max > 10000)
  {
    rb_raise(rb_eRangeError,"out of range (1..10000)");
  }

  return original;
}

VALUE m_s_max(VALUE obj, VALUE num)
{
  VALUE original = default_max_output;

  Check_Type(num, T_FIXNUM);

  default_max_output = num;

  long min = NUM2LONG(default_min_output);
  long max = NUM2LONG(default_max_output);

  if(max < min)
  {
    rb_raise(rb_eException,"upper threshold must be more than lower "
    "threshold (min : %lu, max : %lu)", min, max);
  }

  if(min < 1 || max > 10000)
  {
    rb_raise(rb_eRangeError,"out of range (1..10000)");
  }

  return original;
}

static void c_cwdict_free(void* dict)
{
  cwdict_t* d = (cwdict_t*)dict;
  if(d->drows && d->count > 0) cwdict_close(*d);
}

VALUE c_cwdict_open(int argc, VALUE* argv, VALUE obj)
{
  VALUE file_pathname;
  FILE* file_ptr = NULL;
  cwdict_t d = cwdict_open(), path_parts, file_parts;
  char* tkns_ptr = NULL; char* name_ptr = NULL;
  char* path_name = NULL; char* file_name = NULL;
  char* name = NULL; bool used_file = false; char* file_buffer = NULL;
  long last_drow = 0, last_word = 0; size_t len = 0;

  // raise exception if passed wrong number of arguments
  if(argc > 2)
  rb_raise(rb_eArgError, "wrong number of arguments (expected 0..2)");

  if(argc == 1)
  {
    switch(TYPE(argv[0]))
    {
      case T_FILE:
        #ifdef HAVE_RUBY_IO_H
        file_pathname = RFILE(argv[0])->fptr->pathv;
        path_name = StringValueCStr(file_pathname);
        file_ptr = rb_io_stdio_file(RFILE(argv[0])->fptr);
        #else
        path_name = RFILE(argv[0])->fptr->path;
        file_ptr = RFILE(argv[0])->fptr->f;
        #endif

        if(include(path_name, "/") || include(path_name, "\\"))
        {
          path_parts = split_into_cwdict(path_name, "/\\");

          last_drow = path_parts.count - 1;
          last_word = path_parts.drows[last_drow].count - 1;
          len = strlen(path_parts.drows[last_drow].words[last_word]);

          file_name = (char*)malloc(len + 1);
          strcpy(file_name, path_parts.drows[last_drow].words[last_word]);
          file_name[len] = '\0';

          cwdict_close(path_parts);
        }
        else
        {
          file_name = (char*)malloc(strlen(path_name) + 1);
          strcpy(file_name, path_name);
          file_name[strlen(path_name)] = '\0';
        }

        if(include(file_name, "."))
        {
          file_parts = split_into_cwdict(file_name, ".");

          size_t len = strlen(file_parts.drows[0].words[0]);

          name = (char*)malloc(len + 1);
          strcpy(name, file_parts.drows[0].words[0]);
          name[len] = '\0';

          cwdict_close(file_parts);
        }
        else
        {
          name = (char*)malloc(strlen(file_name) + 1);
          strcpy(name, file_name);
          name[strlen(file_name)] = '\0';
        }

        used_file = true;

        break;
      case T_STRING:
        tkns_ptr = StringValueCStr(argv[0]);
        break;
      default:
        rb_raise(rb_eTypeError, "invalid type (String or File)");
        break;
    }
  }
  if(argc == 2)
  {
    Check_Type(argv[1], T_STRING);
    name_ptr = StringValueCStr(argv[1]);
  }

  // check if tkns references existing, embedded dictionary...
  if(!tkns_ptr && !name_ptr && !used_file) d = cwdict_open();
  else if(tkns_ptr && !used_file)
  {
    if(strcmp(tkns_ptr, "seussian") == 0 || strcmp(tkns_ptr, "Seussian") == 0 ||
    strcmp(tkns_ptr, "seuss") == 0 || strcmp(tkns_ptr, "Seuss") == 0)
    {
      if(!name_ptr)
      d=cwdict_open_with_name_and_tokens("seussian",dict_seuss,CW_DELIMITERS);
      else
      d=cwdict_open_with_name_and_tokens(name_ptr,dict_seuss,CW_DELIMITERS);
    }
    else if(strcmp(tkns_ptr, "latin") == 0 || strcmp(tkns_ptr, "Latin") == 0)
    {
      if(!name_ptr)
      d=cwdict_open_with_name_and_tokens("latin", dict_latin,CW_DELIMITERS);
      else
      d=cwdict_open_with_name_and_tokens(name_ptr, dict_latin,CW_DELIMITERS);
    }
  }
  // ...else, if just a name was passed...
  else if(name_ptr && !used_file) d = cwdict_open_with_name(name_ptr);
  // ...else, see if file exists by passed name...
  else if(used_file && name && file_ptr)
  {
    U32 stringify_result = stringify_file(&file_buffer, file_ptr);
    if(stringify_result == 0)
    {
      rb_raise(rb_eException, "unable to process passed file (%s)",
      file_name);
    }

    d=cwdict_open_with_name_and_tokens(name, file_buffer, CW_DELIMITERS);

    free(file_buffer);
  }
  // ...else, return a blank dictionary

  // create a dictionary pointer
  cwdict_t* d_ptr = (cwdict_t*)malloc(sizeof(cwdict_t));
  *d_ptr = d;

  if(name) free(name);
  if(file_name) free(file_name);

  return Data_Wrap_Struct(c_cwdict, 0, c_cwdict_free, d_ptr);
}

VALUE c_cwdict_close(VALUE obj)
{
  cwdict_t* d;
  cwdict_t empty;

  // get original pointer from Ruby VM and close
  Data_Get_Struct(obj, cwdict_t, d);

  if(d->drows && d->count > 0) { *d = cwdict_close(*d); }

  return obj;
}

VALUE c_cwdict_name_g(VALUE obj)
{
  cwdict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  if(d->name && strlen(d->name) > 0) return rb_str_new2(d->name);
  return rb_str_new2("");
}

VALUE c_cwdict_name_s(VALUE obj, VALUE name)
{
  cwdict_t* d; long len = RSTRING_LEN(name);

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  d->name = (char*)malloc(len + 1);
  strcpy(d->name, StringValueCStr(name));
  d->name[len] = '\0';

  return obj;
}

VALUE c_cwdict_length(VALUE obj)
{
  cwdict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  if(d->count == 0) return INT2NUM(0);
  return LONG2NUM(cwdict_length(*d));
}

VALUE c_cwdict_enum_length(VALUE obj, VALUE args, VALUE eobj)
{
  return c_cwdict_length(obj);
}

VALUE c_cwdict_join(int argc, VALUE* argv, VALUE obj)
{
  cwdict_t* d;

  // raise exception if passed wrong number of arguments
  if(argc > 1) rb_raise(rb_eArgError, "wrong number of arguments");
  if(argc == 1) Check_Type(argv[0], T_STRING);

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  if(argc == 0) return rb_str_new2(cwdict_join(*d, " "));
  return rb_str_new2(cwdict_join(*d, StringValueCStr(argv[0])));
}

VALUE c_cwdict_clone(VALUE obj)
{
  cwdict_t* d, *new_p;
  VALUE new;

  // open new dict for return value
  VALUE args[] = { rb_str_new2(""), rb_str_new2("") };
  new = c_cwdict_open(2, args, new);

  // get original pointers from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);
  Data_Get_Struct(new, cwdict_t, new_p);

  // get a copy of the original dictionary
  *new_p = cwdict_clone(*d);

  return new;
}

VALUE c_cwdict_sample(VALUE obj)
{
  cwdict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  return rb_str_new2(cwdict_sample(*d));
}

VALUE c_cwdict_each(VALUE obj)
{
  cwdict_t *d;

  // get original pointers from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  #ifdef RETURN_SIZE_ENUMERATOR
  RETURN_SIZED_ENUMERATOR(obj, 0, 0, c_cwdict_enum_length);
  #else
  RETURN_ENUMERATOR(obj, 0, 0);
  #endif

  for(U32 i = 0; i != d->count; ++i)
  {
    for(U32 j = 0; j != d->drows[i].count; ++j)
    {
      rb_yield(rb_str_new2(d->drows[i].words[j]));
    }
  }

  return obj;
}

VALUE c_cwdict_each_index(VALUE obj)
{
  long num = 0; cwdict_t *d;

  // get original pointers from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  #ifdef RETURN_SIZE_ENUMERATOR
  RETURN_SIZED_ENUMERATOR(obj, 0, 0, c_cwdict_enum_length);
  #else
  RETURN_ENUMERATOR(obj, 0, 0);
  #endif

  for(U32 i = 0; i != d->count; ++i)
  {
    for(U32 j = 0; j != d->drows[i].count; ++j)
    {
      rb_yield(LONG2NUM(++num));
    }
  }

  return obj;
}

VALUE c_cwdict_map_s(VALUE obj);
VALUE c_cwdict_map(VALUE obj)
{
  VALUE new;

  // get a new copy of the original dict
  new = c_cwdict_clone(obj);
  new = c_cwdict_map_s(new);

  return new;
}

VALUE c_cwdict_map_s(VALUE obj)
{
  cwdict_t *d;

  // get original pointers from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  for(U32 i = 0; i != d->count; ++i)
  {
    for(U32 j = 0; j != d->drows[i].count; ++j)
    {
      VALUE ref = rb_yield(rb_str_new2(d->drows[i].words[j]));
      long len = RSTRING_LEN(ref);

      d->drows[i].words[j] = (char*)realloc(d->drows[i].words[j], len + 1);
      strcpy(d->drows[i].words[j], StringValueCStr(ref));
      d->drows[i].words[j][len] = '\0';
    }
  }

  return obj;
}

VALUE c_cwdict_sort(VALUE obj)
{
  cwdict_t* d;
  VALUE new;

  // get a new copy of the original dict
  new = c_cwdict_clone(obj);

  // get original pointer from Ruby VM
  Data_Get_Struct(new, cwdict_t, d);

  *d = cwdict_sort(*d);

  return new;
}

VALUE c_cwdict_prune(VALUE obj)
{
  cwdict_t* d;
  VALUE new;

  // get a new copy of the original dict
  new = c_cwdict_clone(obj);

  // get original pointer from Ruby VM
  Data_Get_Struct(new, cwdict_t, d);

  *d = cwdict_prune(*d, false);

  return new;
}

VALUE c_cwdict_clean(VALUE obj)
{
  cwdict_t* d;
  VALUE new;

  // get a new copy of the original dict
  new = c_cwdict_clone(obj);

  // get original pointer from Ruby VM
  Data_Get_Struct(new, cwdict_t, d);

  *d = cwdict_prune(*d, true);

  return new;
}

VALUE c_cwdict_sort_s(VALUE obj)
{
  cwdict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  *d = cwdict_sort(*d);

  return obj;
}

VALUE c_cwdict_prune_s(VALUE obj)
{
  cwdict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  *d = cwdict_prune(*d, false);

  return obj;
}

VALUE c_cwdict_clean_s(VALUE obj)
{
  cwdict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  *d = cwdict_prune(*d, true);

  return obj;
}

VALUE c_cwdict_validate_s(VALUE obj)
{
  cwdict_t* d;
  cwerror_t e;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  // handle invalid state first (for error handling's sake)
  if(!cwdict_valid(*d, &e))
  {
    char* message = cwerror_string(*d, e);
    if(e == CWERROR_DICT_TOO_SMALL)
    rb_raise(e_dict_too_small, "%s", message);
    else if(e == CWERROR_DICT_UNSORTABLE)
    rb_raise(e_dict_unsortable, "%s", message);
    else
    rb_raise(e_dict_unknown, "%s", message);

    free(message);
  }

  return obj;
}

VALUE c_cwdict_named_q(VALUE obj)
{
  cwdict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  if(d->name && strlen(d->name) > 0) return Qtrue;
  return Qfalse;
}

VALUE c_cwdict_valid_q(VALUE obj)
{
  cwdict_t* d;
  cwerror_t e;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  // handle invalid state first (for error handling's sake)
  if(!cwdict_valid(*d, &e))
  {
    char* message = cwerror_string(*d, e);
    if(e == CWERROR_DICT_TOO_SMALL)
    rb_raise(e_dict_too_small, "%s", message);
    else if(e == CWERROR_DICT_UNSORTABLE)
    rb_raise(e_dict_unsortable, "%s", message);
    else
    rb_raise(e_dict_unknown, "%s", message);

    free(message);

    return Qfalse;
  }

  return Qtrue;
}

VALUE c_cwdict_sorted_q(VALUE obj)
{
  cwdict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  if(d->sorted) return Qtrue;
  return Qfalse;
}

VALUE c_cwdict_include_q(VALUE obj, VALUE string)
{
  cwdict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  if(cwdict_include(*d, StringValueCStr(string))) return Qtrue;
  return Qfalse;
}

VALUE c_cwdict_exclude_q(VALUE obj, VALUE string)
{
  cwdict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  if(cwdict_exclude(*d, StringValueCStr(string))) return Qtrue;
  return Qfalse;
}

VALUE c_cwdict_inspect(VALUE obj)
{
  cwdict_t* dict; VALUE str;
  size_t count = 0; int multiplier = 1; int word_len = 0;
  char* result = (char*)malloc(CW_LARGE_BUFFER * multiplier + 1);

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, dict);

  // add opening delimiter
  strcpy(result, "["); ++count;

  // add internal dictionary row elements
  for(unsigned long i = 0; i != dict->count; ++i)
  {
    // add opening row delimiter
    strcat(result, "["); ++count;

    for(unsigned long j = 0; j != dict->drows[i].count; ++j)
    {
      // get word length for count
      word_len = strlen(dict->drows[i].words[j]);

      // add word to resulting string
      strcat(result, dict->drows[i].words[j]);
      count += word_len;

      // add continuation delimiter (if applicable)
      if(j < dict->drows[i].count - 1){ strcat(result,", "); count += 2; }

      // resize result if necessary
      if(count >= CW_LARGE_BUFFER * multiplier - 100)
      {
        // create temporary copy
        char* temp = (char*)malloc(CW_LARGE_BUFFER * multiplier + 1);

        result[count] = '\0';
        strcpy(temp, result);
        temp[count] = '\0';

        // increase buffer size
        result = (char*)realloc(result,CW_LARGE_BUFFER * ++multiplier + 1);

        // move back into resulting string and clear intermediary buffer
        strcpy(result, temp);
        result[count] = '\0';

        free(temp);
      }
    }

    // add closing row delimiter
    strcat(result, "]"); ++count;

    // add continuation delimiter (if applicable)
    if(i < dict->count - 1) { strcat(result, ", "); count += 2; }
  }

  // add closing delimiter
  strcat(result, "]"); ++count;

  // NULL terminator (just in case)
  result[count] = '\0';

  str = rb_str_new2(result);
  free(result);

  return str;
}

VALUE c_cwdict_to_s(VALUE obj)
{
  cwdict_t* dict;
  size_t count = 0; int multiplier = 1;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, dict);

  // if(dict->drows && dict->count > 0) return Qnil;
  return c_cwdict_inspect(obj);
}

VALUE c_cwdict_append_op(VALUE obj, VALUE addend)
{
  cwdict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);

  switch(TYPE(addend))
  {
    case T_STRING:
      *d = cwdict_place_word_strict(*d, StringValueCStr(addend));
      break;
    case T_ARRAY:
      for(long i = 0; i != RARRAY_LEN(addend); ++i)
      {
        if(TYPE(rb_ary_entry(addend, i)) != T_STRING)
        {
          rb_raise(rb_eTypeError,"not a valid value (passed Array can only"
          " contain String elements)");
          break;
        }

        VALUE entry = rb_ary_entry(addend, i);
        char* entry_str = StringValueCStr(entry);

        *d = cwdict_place_word_strict(*d, entry_str);
      }

      break;
    default:
      rb_raise(rb_eTypeError,"not a valid value (expected String or Array");
      break;
  }

  if(d->sorted) return c_cwdict_sort(obj);
  return obj;
}

VALUE c_cwdict_add_op(VALUE obj, VALUE addend)
{
  VALUE new;

  // get a clone of the original
  new = c_cwdict_clone(obj);

  return c_cwdict_append_op(new, addend);
}

VALUE c_cwdict_add_assign_op(VALUE obj, VALUE addend)
{
  return c_cwdict_append_op(obj, addend);
}

VALUE c_cwdict_check_equality(VALUE obj, VALUE against)
{
  cwdict_t* d, *comparison;

  // get original pointers from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);
  Data_Get_Struct(against, cwdict_t, comparison);

  if(cwdict_equal(*d, *comparison)) return Qtrue;
  return Qfalse;
}

VALUE c_cwdict_check_inequality(VALUE obj, VALUE against)
{
  cwdict_t* d, *comparison;

  // get original pointers from Ruby VM
  Data_Get_Struct(obj, cwdict_t, d);
  Data_Get_Struct(against, cwdict_t, comparison);

  if(cwdict_inequal(*d, *comparison)) return Qtrue;
  return Qfalse;
}

void Init_chinwag()
{
  // setup module extension and containing class(es)
  m_chinwag = rb_define_module("Chinwag");
  c_cwdict = rb_define_class_under(m_chinwag, "CWDict", rb_cObject);

  // setup exception subclasses for error handling
  e_invalid_output_type =
  rb_define_class("CWErrorInvalidOutputType", rb_eException);
  e_min_less_than_one = rb_define_class("CWErrorMinLessThanOne", rb_eException);
  e_max_less_than_min = rb_define_class("CWErrorMaxLessThanMin", rb_eException);
  e_max_too_high = rb_define_class("CWErrorMaxTooHigh", rb_eException);
  e_dict_too_small = rb_define_class("CWErrorDictTooSmall", rb_eException);
  e_dict_unsortable = rb_define_class("CWErrorDictUnsortable", rb_eException);
  e_dict_unknown = rb_define_class("CWErrorDictUnknown", rb_eException);

  // note module inclusions
  rb_include_module(c_cwdict, rb_mEnumerable);

  // sync up module generation functions
  rb_define_module_function(m_chinwag, "generate", m_cw_generate, -1);
  rb_define_module_function(m_chinwag, "set_default_dict", m_set_d_dict, 1);
  rb_define_module_function(m_chinwag, "set_default_type", m_set_d_type, 1);
  rb_define_module_function(m_chinwag, "set_default_min_output",m_s_min,1);
  rb_define_module_function(m_chinwag, "set_default_max_output",m_s_max,1);

  // sync up class methods
  rb_define_singleton_method(c_cwdict, "open", c_cwdict_open, -1);
  rb_define_method(c_cwdict, "close", c_cwdict_close, 0);

  rb_define_method(c_cwdict, "name", c_cwdict_name_g, 0);
  rb_define_method(c_cwdict, "name=", c_cwdict_name_s, 1);
  rb_define_method(c_cwdict, "length", c_cwdict_length, 0);

  rb_define_method(c_cwdict, "join", c_cwdict_join, -1);
  rb_define_method(c_cwdict, "clone", c_cwdict_clone, 0);
  rb_define_method(c_cwdict, "sample", c_cwdict_sample, 0);

  rb_define_method(c_cwdict, "each", c_cwdict_each, 0);
  rb_define_method(c_cwdict, "each_index", c_cwdict_each_index, 0);
  rb_define_method(c_cwdict, "map", c_cwdict_map, 0);
  rb_define_method(c_cwdict, "map!", c_cwdict_map_s, 0);

  rb_define_method(c_cwdict, "sort", c_cwdict_sort, 0);
  rb_define_method(c_cwdict, "prune", c_cwdict_prune, 0);
  rb_define_method(c_cwdict, "clean", c_cwdict_clean, 0);

  rb_define_method(c_cwdict, "sort!", c_cwdict_sort_s, 0);
  rb_define_method(c_cwdict, "prune!", c_cwdict_prune_s, 0);
  rb_define_method(c_cwdict, "clean!", c_cwdict_clean_s, 0);
  rb_define_method(c_cwdict, "validate!", c_cwdict_validate_s, 0);

  rb_define_method(c_cwdict, "named?", c_cwdict_named_q, 0);
  rb_define_method(c_cwdict, "valid?", c_cwdict_valid_q, 0);
  rb_define_method(c_cwdict, "sorted?", c_cwdict_sorted_q, 0);
  rb_define_method(c_cwdict, "include?", c_cwdict_include_q, 1);
  rb_define_method(c_cwdict, "exclude?", c_cwdict_exclude_q, 1);

  rb_define_method(c_cwdict, "inspect", c_cwdict_inspect, 0);
  rb_define_method(c_cwdict, "to_s", c_cwdict_to_s, 0);

  // operator methods
  rb_define_method(c_cwdict, "+", c_cwdict_add_op, 1);
  rb_define_method(c_cwdict, "+=", c_cwdict_add_assign_op, 1);
  rb_define_method(c_cwdict, "<<", c_cwdict_append_op, 1);
  rb_define_method(c_cwdict, "==", c_cwdict_check_equality, 1);
  rb_define_method(c_cwdict, "!=", c_cwdict_check_inequality, 1);

  // method aliases
  rb_define_alias(c_cwdict, "dup", "clone");
  rb_define_alias(c_cwdict, "size", "length");
  rb_define_alias(c_cwdict, "count", "length");

  // open Seussian dict as default fall-back
  VALUE args[] = { rb_str_new2("seussian") };
  default_dict = c_cwdict_open(1, args, default_dict);

  // set default output type
  default_output_type = ID2SYM(rb_intern("words"));

  // set default minimum output amount
  default_min_output = LONG2NUM(CW_DEFAULT_MIN_OUTPUT);

  // set default maximum output amount
  default_max_output = LONG2NUM(CW_DEFAULT_MAX_OUTPUT);
}
