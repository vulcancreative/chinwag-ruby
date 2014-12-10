#include "rb_chinwag_ext.h"

VALUE m_chinwag;
VALUE c_cw_dict;

// generates based on fixed input
// VALUE cw_generate_fxd(VALUE obj, VALUE type, VALUE amt, VALUE dict)
// {
  // make sure we got Fixnums
  // Check_Type(type, T_FIXNUM);
  // Check_Type(amt, T_FIXNUM);

  // unsigned cw_type = NUM2INT(type);
  // unsigned long amount = NUM2LONG(amt);
  // TODO : get dict as dictionary
  
  // TODO : do check to make sure type <= 3
  // TODO : do check to make sure amt <= 10000
  
  // return rb_str_new_cstr(chinwag(cw_type, amount, amount, dictionary))
// }

// generates based on passed range
// VALUE cw_generate_rng(VALUE obj, VALUE type, VALUE rng, VALUE, dict)
// {

// }

static void c_cw_dict_free(void* dict)
{
  close_dict(*(dict_t*)dict);
  free(dict);
}

VALUE c_cw_dict_open(VALUE obj, VALUE tkns, VALUE name)
{
  // make sure our types are correct
  Check_Type(tkns, T_STRING);
  Check_Type(name, T_STRING);

  dict_t d;
  char* tkns_ptr = StringValueCStr(tkns);
  char* name_ptr = StringValueCStr(name);

  // check if tkns references existing, embedded dictionary...
  if(strcmp(name_ptr, "seussian") == 0)
  {
    d = open_dict_with_name_and_tokens("seussian", dict_seuss, DELIMITERS);
  }
  else if(strcmp(name_ptr, "latin") == 0)
  {
    d = open_dict_with_name_and_tokens("latin", dict_latin, DELIMITERS);
  }
  // TODO : ...else, see if file exists by passed name...
  // TODO : ...else, throw a fit (error)

  // create a dictionary pointer
  dict_t* d_ptr = (dict_t*)malloc(sizeof(dict_t));
  d_ptr = (dict_t*)memmove(d_ptr, &d, sizeof(dict_t));

  return Data_Wrap_Struct(rb_cObject, 0, c_cw_dict_free, d_ptr);
}

VALUE c_cw_dict_close(VALUE obj)
{
  dict_t* d;

  // get original pointer from Ruby VM and close
  Data_Get_Struct(obj, dict_t, d);
  c_cw_dict_free(d);

  return Qnil;
}

VALUE c_cw_dict_to_s(VALUE obj)
{
  dict_t* dict;
  size_t count = 0; int multiplier = 1;
  VALUE result = rb_str_new("", LARGE_BUFFER * multiplier + 1);

  rb_str_set_len(result, LARGE_BUFFER * multiplier);

  // get original pointer from Ruby VM
  Data_Get_Struct(obj, dict_t, dict);

  // add opening delimiter
  rb_str_cat2(result, "[");

  // add internal dictionary row elements
  for(unsigned long i = 0; i != dict->count; ++i)
  {
    rb_str_cat2(result, "[");
    count += 1;

    for(unsigned long j = 0; j != dict->drows[i].count; ++j)
    {
      rb_str_cat2(result, dict->drows[i].words[j]);
      if(j < dict->drows[i].count - 1) rb_str_cat2(result, ",");

      count += strlen(dict->drows[i].words[j]) + 1;
      if(count >= (LARGE_BUFFER * multiplier) - 100)
      {
        ++multiplier;
        rb_str_resize(result, LARGE_BUFFER * multiplier + 1);
        rb_str_set_len(result, LARGE_BUFFER * multiplier);
      }
    }

    rb_str_cat2(result, "]");
    if(i < dict->count - 1) rb_str_cat2(result, ",");
  }

  // add closing delimiter
  rb_str_cat2(result, "]");
  rb_str_cat2(result, "\0");

  return result;
}

void Init_chinwag()
{
  // setup module extension and containing class(es)
  m_chinwag = rb_define_module("Chinwag");
  c_cw_dict = rb_define_class_under(m_chinwag, "CWDict", rb_cObject);

  // define constants
  rb_define_const(m_chinwag, "LETTERS", INT2NUM(0));
  rb_define_const(m_chinwag, "WORDS", INT2NUM(1));
  rb_define_const(m_chinwag, "SENTENCES", INT2NUM(2));
  rb_define_const(m_chinwag, "PARAGRAPHS", INT2NUM(3));

  // sync up module generation functions
  // rb_define_module_function(m_chinwag, "generate", m_cw_generate_fxd, 3);
  // rb_define_module_function(m_chinwag, "generate", m_cw_generate_rng, 3);
  
  // sync up class methods
  rb_define_method(c_cw_dict, "open", c_cw_dict_open, 2);
  rb_define_method(c_cw_dict, "close", c_cw_dict_close, 0);
  // rb_define_method(c_cw_dict, "name", c_cw_dict_name, 1);
  // rb_define_method(c_cw_dict, "length", c_cw_dict_length, 0);
  // rb_define_method(c_cw_dict, "sort", c_cw_dict_sort, 0);
  // rb_define_method(c_cw_dict, "prune", c_cw_dict_prune, 0);
  // rb_define_method(c_cw_dict, "join", c_cw_dict_join, 1);
  // rb_define_method(c_cw_dict, "sample", c_cw_dict_sample);
  // rb_define_method(c_cw_dict, "validate", c_cw_dict_validate, 0);
  // rb_define_method(c_cw_dict, "named?", c_cw_dict_named_q, 0);
  // rb_define_method(c_cw_dict, "valid?", c_cw_dict_valid_q, 0);
  // rb_define_method(c_cw_dict, "include?", c_cw_dict_include_q, 1);
  // rb_define_method(c_cw_dict, "exclude?", c_cw_dict_exclude_q, 1);
  rb_define_method(c_cw_dict, "to_s", c_cw_dict_to_s, 0);

  // rb_define_alias(c_cw_dict, "size", "length");
}
