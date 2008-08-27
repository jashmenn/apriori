// Include the Ruby headers and goodies
#include "ruby.h"
#include "apriori_wrapper.h"

// Defining a space for information and references about the module to be stored internally
VALUE Apriori = Qnil;

// Prototype for the initialization method - Ruby calls this, not you
void Init_mytest();

// Prototype for our method 'test1' - methods are prefixed by 'method_' here
VALUE method_do_apriori(VALUE self);
VALUE method_test_hash_ap(VALUE self, VALUE filein, VALUE fileout, VALUE opts);
VALUE method_find_association_rules(VALUE self, VALUE filein, VALUE fileout, VALUE opts);
VALUE method_ap_test_ruby_array(VALUE self, VALUE rargv); 

// The initialization method for this module
void Init_apriori() {
	Apriori = rb_define_module("Apriori");
	rb_define_method(Apriori, "do_apriori",   method_do_apriori, 0);	
	rb_define_method(Apriori, "test_hash_ap", method_test_hash_ap, 3);
	rb_define_method(Apriori, "find_association_rules", method_find_association_rules, 3);
}

VALUE method_do_apriori(VALUE self) {
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

// Our 'test1' method.. it simply returns a value of '10' for now.
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

VALUE method_ap_test_ruby_array(VALUE self, VALUE rargv) {
  // get an array of ruby strings
  // convert it to a c array of char *'s
    VALUE s = rb_str_new2("hello");
    //VALUE s = rb_str_new2("bye");
    //VALUE val = rb_hash_aref(opts, rb_str_intern(s));

    /* if(val != Qnil) { */
    /*   return val; */
    /* } else { */
    /*   return rb_str_new2("nope"); */
    /* } */
    return Qnil;
}
