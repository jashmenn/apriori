// Include the Ruby headers and goodies
#include "ruby.h"
#include "intern.h"
#include "apriori_wrapper.h"

// Defining a space for information and references about the module to be stored internally
VALUE Apriori = Qnil;

// Prototype for the initialization method - Ruby calls this, not you
void Init_mytest();

// Prototype for our method 'test1' - methods are prefixed by 'method_' here
VALUE method_do_test_apriori(VALUE self);
//VALUE method_test_hash_ap(VALUE self, VALUE filein, VALUE fileout, VALUE opts);
VALUE method_find_association_rules(VALUE self, VALUE filein, VALUE fileout, VALUE opts);
VALUE method_ap_do_apriori(VALUE self, VALUE rargv); 

// The initialization method for this module
void Init_apriori_ext() {
	Apriori = rb_define_module("Apriori");
	rb_define_method(Apriori, "do_test_apriori",   method_do_test_apriori, 0);	
	//rb_define_method(Apriori, "test_hash_ap", method_test_hash_ap, 3);
	rb_define_method(Apriori, "do_apriori", method_ap_do_apriori, 1);
}

VALUE method_do_test_apriori(VALUE self) {
    char *arg0 = "apriori";
    char *arg1 = "test/sample.txt";
    char *arg2 = "test/results.txt";
    char *argv[3];
    argv[0] = arg0;
    argv[1] = arg1;
    argv[2] = arg2;

    do_apriori(3, argv);
	int x = 10;
	return INT2NUM(x);
}

// Our 'test1' method.. it simply returns a value of '10' for now.
/*
VALUE method_test_hash_ap(VALUE self, VALUE filein, VALUE fileout, VALUE opts) {
    //VALUE s = rb_str_new2("hello");
    VALUE s = rb_str_new2("bye");
    VALUE val = rb_hash_aref(opts, rb_str_intern(s));

    if(val != Qnil) {
      return val;
    } else {
      return rb_str_new2("nope");
    }
}
*/

// Our 'test1' method.. it simply returns a value of '10' for now.
/*
VALUE method_find_association_rules(VALUE self, VALUE filein, VALUE fileout, VALUE opts) {
    VALUE s = rb_str_new2("hello");
    //VALUE s = rb_str_new2("bye");
    VALUE val = rb_hash_aref(opts, rb_str_intern(s));

    if(val != Qnil) {
      return val;
    } else {
      return rb_str_new2("nope");
    }
}
*/

VALUE method_ap_do_apriori(VALUE self, VALUE rargv) {
  char **argv; // todo, learn the best practice here
  int i = 0;
  VALUE ary_value;
  // get the number of arguments
  int ruby_argv_length = RARRAY(rargv)->len;
  // allocate enough memory for a pointer to each char *
  argv = malloc(ruby_argv_length * sizeof(*argv));

  // copy the ruby array of strings to a c "array of strings"
  // todo, turn this into a function
  while((ary_value = rb_ary_shift(rargv)) != Qnil) {
    argv[i] = malloc(strlen(STR2CSTR(ary_value)) + 1);
    strcpy(argv[i], STR2CSTR(ary_value));
    i++;
  }
  do_apriori(i, argv);
  return Qnil;
}

/*
VALUE better_method_ap_test_ruby_array(VALUE self, VALUE rargv) {
  char **argv;
  int i = 3;
  argv = (char **)convert_rb_ary_strings_to_c_ary_strings(rargv);
  do_apriori(i, argv);
}

char **convert_rb_ary_strings_to_c_ary_strings(VALUE rary) {
  char **argv; // todo, learn the best practice here
  int i = 0;
  VALUE ary_value;
  // get the number of arguments
  int ruby_argv_length = RARRAY(rary)->len;
  // allocate enough memory for a pointer to each char *
  argv = malloc(ruby_argv_length * sizeof(*argv));

  // copy the ruby array of strings to a c "array of strings"
  // todo, turn this into a function
  while((ary_value = rb_ary_shift(rary)) != Qnil) {
    argv[i] = malloc(strlen(STR2CSTR(ary_value)) + 1);
    strcpy(argv[i], STR2CSTR(ary_value));
    fprintf(stderr, "%s\n", argv[i]);
    i++;
  }
  printf("%d\n", i);
  return argv;
}
*/

void asdf_junk_box() {
  /* int j; */
  /* for(j=0; j<i; j++) { */
  /*   fprintf(stderr, "%s\n", argv[j]); */
  /* } */

  // VALUE tmp_argv = rb_ary_new();
  //do_apriori(i, argv);

  // ok, so you need to figure out how many non-nil values you have,
  // i guess you could just track it above.
  // then we're good! we can feed these to the main

//  for(i=0; i < NUM2INT(rb_ary_length(rargv)); i++) {
//    VALUE argv = rb_ary_new();
    //fprintf(stderr, STR2CSTR(rb_ary_fetch(1, 1, rargv)));
//  }
    // get an array of ruby strings
    // convert it to a c array of char *'s
    //
    //VALUE s = rb_str_new2("bye");
    //VALUE val = rb_hash_aref(opts, rb_str_intern(s));

    /* if(val != Qnil) { */
    /*   return val; */
    /* } else { */
    /*   return rb_str_new2("nope"); */
    /* } */
  return;
}
