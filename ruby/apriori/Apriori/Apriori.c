// Include the Ruby headers and goodies
#include "ruby.h"
#include "apriori_wrapper.h"

// Defining a space for information and references about the module to be stored internally
VALUE Apriori = Qnil;

// Prototype for the initialization method - Ruby calls this, not you
void Init_mytest();

// Prototype for our method 'test1' - methods are prefixed by 'method_' here
VALUE method_test1(VALUE self);
VALUE method_help(VALUE self);
VALUE method_do_apriori(VALUE self);

// The initialization method for this module
void Init_apriori() {
	Apriori = rb_define_module("Apriori");
	rb_define_method(Apriori, "test1", method_test1, 0);	
	rb_define_method(Apriori, "help", method_help, 0);	
	rb_define_method(Apriori, "do_apriori", method_do_apriori, 0);	
}

// Our 'test1' method.. it simply returns a value of '10' for now.
VALUE method_test1(VALUE self) {
	int x = 10;
	return INT2NUM(x);
}

VALUE method_help(VALUE self) {
	int x = 10;
    help();
	return INT2NUM(x);
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
