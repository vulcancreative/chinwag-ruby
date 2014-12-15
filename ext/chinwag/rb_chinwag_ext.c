#include "rb_chinwag_ext.h"

VALUE m_chinwag;
VALUE c_cw_dict;

VALUE default_dict;
VALUE default_output_type;

cw_t get_cw_t_for_symbol(VALUE symbol)
{
  cw_t cw_type = CW_DEFAULT;

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
  dict_t* d;
  VALUE result;
  cw_t cw_type = get_cw_t_for_symbol(default_output_type);
  long min = DEFAULT_MIN_OUTPUT_LENGTH, max = DEFAULT_MAX_OUTPUT_LENGTH;
  char* output, *e;

  // raise exception if passed wrong number of arguments
  if(argc > 4)
  {
    rb_raise(rb_eArgError, "wrong number of arguments (0..4)");
  }

  // do stuff with 'em
  if(argc == 0) Data_Get_Struct(default_dict, dict_t, d);

  if(argc >= 1) Data_Get_Struct(argv[0], dict_t, d);
  
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

  if(max < min)
  {
    rb_raise(rb_eException,"upper threshold must be more than lower "
    "threshold (min : %lu, max : %lu)", min, max);
  }

  if(min < 1 || max > 10000)
  {
    rb_raise(rb_eRangeError,"out of range (1..10000)");
  }

  if(!dict_valid(*d, &e))
  {
    rb_raise(rb_eException, "%s", e);
    free(e);
  }

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

  result = rb_str_new2(output);
  // free(output);

  return result;
}

VALUE c_cw_dict_clone(VALUE obj);
VALUE c_cw_dict_close(VALUE obj);
VALUE m_set_d_dict(VALUE obj, VALUE new)
{
  VALUE original = c_cw_dict_clone(default_dict);

  default_dict = c_cw_dict_close(default_dict);
  default_dict = c_cw_dict_clone(new);

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

static void c_cw_dict_free(void* dict)
{
  dict_t* d = (dict_t*)dict;
  if(d->drows && d->count > 0) close_dict(*d);
}

VALUE c_cw_dict_open(int argc, VALUE* argv, VALUE obj)
{
  VALUE file_pathname;
  FILE* file_ptr = NULL;
  dict_t d = open_dict(), path_parts, file_parts;
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
          path_parts = split(path_name, "/\\");

          last_drow = path_parts.count - 1;
          last_word = path_parts.drows[last_drow].count - 1;
          len = strlen(path_parts.drows[last_drow].words[last_word]);

          file_name = (char*)malloc(len + 1);
          strcpy(file_name, path_parts.drows[last_drow].words[last_word]);
          file_name[len] = '\0';

          close_dict(path_parts);
        }
        else
        {
          file_name = (char*)malloc(strlen(path_name) + 1);
          strcpy(file_name, path_name);
          file_name[strlen(path_name)] = '\0';
        }

        if(include(file_name, "."))
        {
          file_parts = split(file_name, ".");

          size_t len = strlen(file_parts.drows[0].words[0]);

          name = (char*)malloc(len + 1);
          strcpy(name, file_parts.drows[0].words[0]);
          name[len] = '\0';

          close_dict(file_parts);
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
  if(!tkns_ptr && !name_ptr && !used_file) d = open_dict();
  else if(tkns_ptr && !used_file)
  {
    if(strcmp(tkns_ptr, "seussian") == 0)
    {
      if(!name_ptr)
      d = open_dict_with_name_and_tokens("seussian",dict_seuss,DELIMITERS);
      else
      d = open_dict_with_name_and_tokens(name_ptr,dict_seuss,DELIMITERS);
    }
    else if(strcmp(tkns_ptr, "latin") == 0)
    {
      if(!name_ptr)
      d = open_dict_with_name_and_tokens("latin", dict_latin,DELIMITERS);
      else
      d = open_dict_with_name_and_tokens(name_ptr, dict_latin,DELIMITERS);
    }
  }
  // ...else, if just a name was passed...
  else if(name_ptr && !used_file) d = open_dict_with_name(name_ptr);
  // ...else, see if file exists by passed name...
  else if(used_file && name && file_ptr)
  {
    U32 stringify_result = stringify_file(&file_buffer, file_ptr);
    if(stringify_result == 0)
    {
      rb_raise(rb_eException, "unable to process passed file (%s)", 
      file_name);
    }

    d = open_dict_with_name_and_tokens(name, file_buffer, DELIMITERS);

    free(file_buffer);
  }
  // ...else, return a blank dictionary

  // create a dictionary pointer
  dict_t* d_ptr = (dict_t*)malloc(sizeof(dict_t));
  *d_ptr = d;

  if(name) free(name);
  if(file_name) free(file_name);

  return Data_Wrap_Struct(c_cw_dict, 0, c_cw_dict_free, d_ptr);
}

VALUE c_cw_dict_close(VALUE obj)
{
  dict_t* d;
  dict_t empty;

  // get original pointer from Ruby VM and close
  Data_Get_Struct(obj, dict_t, d);

  if(d->drows && d->count > 0) { *d = close_dict(*d); }
  
  return obj;
}

VALUE c_cw_dict_name_g(VALUE obj)
{
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  if(d->name && strlen(d->name) > 0) return rb_str_new2(d->name);
  return rb_str_new2("");
}

VALUE c_cw_dict_name_s(VALUE obj, VALUE name)
{
  dict_t* d; long len = RSTRING_LEN(name);

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  d->name = (char*)malloc(len + 1);
  strcpy(d->name, StringValueCStr(name));
  d->name[len] = '\0';

  return obj;
}

VALUE c_cw_dict_length(VALUE obj)
{
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  if(d->count == 0) return INT2NUM(0);
  return LONG2NUM(total_dict(*d));
}

VALUE c_cw_dict_join(int argc, VALUE* argv, VALUE obj)
{
  dict_t* d;

  // raise exception if passed wrong number of arguments
  if(argc > 1) rb_raise(rb_eArgError, "wrong number of arguments");
  if(argc == 1) Check_Type(argv[0], T_STRING);

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  if(argc == 0) return rb_str_new2(join_dict(*d, " "));
  return rb_str_new2(join_dict(*d, StringValueCStr(argv[0])));
}

VALUE c_cw_dict_clone(VALUE obj)
{
  dict_t* d, *new_p;
  VALUE new;

  // open new dict for return value
  VALUE args[] = { rb_str_new2(""), rb_str_new2("") };
  new = c_cw_dict_open(2, args, new);

  // get original pointers from Ruby VM
  Data_Get_Struct(obj, dict_t, d);
  Data_Get_Struct(new, dict_t, new_p);

  // get a copy of the original dictionary
  *new_p = deep_copy_dict(*d);

  return new;
}

VALUE c_cw_dict_sample(VALUE obj)
{
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  return rb_str_new2(sample_dict(*d));
}

VALUE c_cw_dict_sort(VALUE obj)
{
  dict_t* d;
  VALUE new;

  // get a new copy of the original dict
  new = c_cw_dict_clone(obj);

  // get original pointer from Ruby VM
  Data_Get_Struct(new, dict_t, d);

  *d = bubble_dict(*d);

  return new;
}

VALUE c_cw_dict_prune(VALUE obj)
{
  dict_t* d;
  VALUE new;

  // get a new copy of the original dict
  new = c_cw_dict_clone(obj);

  // get original pointer from Ruby VM
  Data_Get_Struct(new, dict_t, d);

  *d = prune_dict(*d, false);

  return new;
}

VALUE c_cw_dict_clean(VALUE obj)
{
  dict_t* d;
  VALUE new;

  // get a new copy of the original dict
  new = c_cw_dict_clone(obj);

  // get original pointer from Ruby VM
  Data_Get_Struct(new, dict_t, d);

  *d = prune_dict(*d, true);

  return new;
}

VALUE c_cw_dict_sort_s(VALUE obj)
{
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  *d = bubble_dict(*d);

  return obj;
}

VALUE c_cw_dict_prune_s(VALUE obj)
{
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  *d = prune_dict(*d, false);

  return obj;
}

VALUE c_cw_dict_clean_s(VALUE obj)
{
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  *d = prune_dict(*d, true);

  return obj;
}

VALUE c_cw_dict_validate_s(VALUE obj)
{
  char* e;
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  // handle invalid state first (for error handling's sake)
  if(!dict_valid(*d, &e))
  {
    rb_raise(rb_eException, "%s", e);
    free(e);
  }

  return obj;
}

VALUE c_cw_dict_named_q(VALUE obj)
{
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  if(d->name && strlen(d->name) > 0) return Qtrue;
  return Qfalse;
}

VALUE c_cw_dict_valid_q(VALUE obj)
{
  char* e;
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  // handle invalid state first (for error handling's sake)
  if(!dict_valid(*d, &e))
  {
    rb_raise(rb_eException, "%s", e);
    free(e);

    return Qfalse;
  }

  return Qtrue;
}

VALUE c_cw_dict_sorted_q(VALUE obj)
{
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  if(d->sorted) return Qtrue;
  return Qfalse;
}

VALUE c_cw_dict_include_q(VALUE obj, VALUE string)
{
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  if(dict_include(*d, StringValueCStr(string))) return Qtrue;
  return Qfalse;
}

VALUE c_cw_dict_exclude_q(VALUE obj, VALUE string)
{
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  if(dict_exclude(*d, StringValueCStr(string))) return Qtrue;
  return Qfalse;
}

VALUE c_cw_dict_inspect(VALUE obj)
{
  dict_t* dict; VALUE str;
  size_t count = 0; int multiplier = 1; int word_len = 0;
  char* result = (char*)malloc(LARGE_BUFFER * multiplier + 1);

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, dict);

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
      if(count >= LARGE_BUFFER * multiplier - 100)
      {
        // create temporary copy
        char* temp = (char*)malloc(LARGE_BUFFER * multiplier + 1);

        result[count] = '\0';
        strcpy(temp, result);
        temp[count] = '\0';

        // increase buffer size
        result = (char*)realloc(result,LARGE_BUFFER * ++multiplier + 1);

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

VALUE c_cw_dict_to_s(VALUE obj)
{
  dict_t* dict;
  size_t count = 0; int multiplier = 1;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, dict);

  // if(dict->drows && dict->count > 0) return Qnil;
  return c_cw_dict_inspect(obj);
}

VALUE c_cw_dict_append_op(VALUE obj, VALUE addend)
{
  dict_t* d;

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, d);

  switch(TYPE(addend))
  {
    case T_STRING:
      *d = place_word_in_dict_strict(*d, StringValueCStr(addend));
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

        *d = place_word_in_dict_strict(*d, entry_str);
      }

      break;
    default:
      rb_raise(rb_eTypeError,"not a valid value (expected String or Array");
      break;
  }

  if(d->sorted) return c_cw_dict_sort(obj);
  return obj;
}

VALUE c_cw_dict_add_op(VALUE obj, VALUE addend)
{
  VALUE new;

  // get a clone of the original
  new = c_cw_dict_clone(obj);

  return c_cw_dict_append_op(new, addend);
}

VALUE c_cw_dict_add_assign_op(VALUE obj, VALUE addend)
{
  return c_cw_dict_append_op(obj, addend);
}

void Init_chinwag()
{
  // setup module extension and containing class(es)
  m_chinwag = rb_define_module("Chinwag");
  c_cw_dict = rb_define_class_under(m_chinwag, "CWDict", rb_cObject);

  // sync up module generation functions
  rb_define_module_function(m_chinwag, "generate", m_cw_generate, -1);
  rb_define_module_function(m_chinwag, "set_default_dict", m_set_d_dict, 1);
  rb_define_module_function(m_chinwag, "set_default_type", m_set_d_type, 1);
  
  // sync up class methods
  rb_define_singleton_method(c_cw_dict, "open", c_cw_dict_open, -1);
  rb_define_method(c_cw_dict, "close", c_cw_dict_close, 0);

  rb_define_method(c_cw_dict, "name", c_cw_dict_name_g, 0);
  rb_define_method(c_cw_dict, "name=", c_cw_dict_name_s, 1);
  rb_define_method(c_cw_dict, "length", c_cw_dict_length, 0);

  rb_define_method(c_cw_dict, "join", c_cw_dict_join, -1);
  rb_define_method(c_cw_dict, "clone", c_cw_dict_clone, 0);
  rb_define_method(c_cw_dict, "sample", c_cw_dict_sample, 0);

  rb_define_method(c_cw_dict, "sort", c_cw_dict_sort, 0);
  rb_define_method(c_cw_dict, "prune", c_cw_dict_prune, 0);
  rb_define_method(c_cw_dict, "clean", c_cw_dict_clean, 0);

  rb_define_method(c_cw_dict, "sort!", c_cw_dict_sort_s, 0);
  rb_define_method(c_cw_dict, "prune!", c_cw_dict_prune_s, 0);
  rb_define_method(c_cw_dict, "clean!", c_cw_dict_clean_s, 0);
  rb_define_method(c_cw_dict, "validate!", c_cw_dict_validate_s, 0);

  rb_define_method(c_cw_dict, "named?", c_cw_dict_named_q, 0);
  rb_define_method(c_cw_dict, "valid?", c_cw_dict_valid_q, 0);
  rb_define_method(c_cw_dict, "sorted?", c_cw_dict_sorted_q, 0);
  rb_define_method(c_cw_dict, "include?", c_cw_dict_include_q, 1);
  rb_define_method(c_cw_dict, "exclude?", c_cw_dict_exclude_q, 1);

  rb_define_method(c_cw_dict, "inspect", c_cw_dict_inspect, 0);
  rb_define_method(c_cw_dict, "to_s", c_cw_dict_to_s, 0);

  // operator methods
  rb_define_method(c_cw_dict, "+", c_cw_dict_add_op, 1);
  rb_define_method(c_cw_dict, "+=", c_cw_dict_add_assign_op, 1);
  rb_define_method(c_cw_dict, "<<", c_cw_dict_append_op, 1);
  // rb_define_method(c_cw_dict, "==", c_cw_dict_check_equality, 1);
  // rb_define_method(c_cw_dict, "!=", c_cw_dict_check_inequality, 1);

  // method aliases
  rb_define_alias(c_cw_dict, "dup", "clone");
  rb_define_alias(c_cw_dict, "size", "length");
  rb_define_alias(c_cw_dict, "count", "length");

  // open Seussian dict as default fall-back
  VALUE args[] = { rb_str_new2("seussian") };
  default_dict = c_cw_dict_open(1, args, default_dict);

  // set default output type
  default_output_type = ID2SYM(rb_intern("words"));
}
